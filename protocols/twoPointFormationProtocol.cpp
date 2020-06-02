#include "twoPointFormationProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>



bool TwoPointFormationProtocol::tick()
{
    //cout<<"tpf tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::STANDING_BEACON:
        return   standingBeaconTick();
        break;
    case RoleInProtocol::MOVING_BEACON:
        return   movingBeaconTick();
        break;

    default:
        break;
    }
    return false;
}

TwoPointFormationProtocol::~TwoPointFormationProtocol()
{

}

void TwoPointFormationProtocol::start()
{
    switch (roleInProtocol) {
    case RoleInProtocol::STANDING_BEACON:// send query for confirmation of protocol start to still beacon
        state = ProtocolStates::WAITING_START_REQUEST;
        break;
    case RoleInProtocol::MOVING_BEACON:{
        VSMMessage startRequest(behaviour->owner->id,Topics::TWO_POINT_FORMATION_TO_STILL,MessageContents::TPF_START,"r");
        behaviour->owner->sendMsg(startRequest);
        state = ProtocolStates::WAITING_REPLY;
    }
    default:
        break;
    }
}


bool TwoPointFormationProtocol::standingBeaconTick()
{
    //wait for request from movingBeacon to start, then stay in still mode
    switch (state) {
    case ProtocolStates::WAITING_START_REQUEST:{
        VSMMessage* res = behaviour->receive(MessageContents::TPF_START);
        if(res!= 0){
            state= ProtocolStates::STARTED;
            VSMMessage reply(behaviour->owner->id,Topics::TWO_POINT_FORMATION_TO_MOVING,MessageContents::AGREE,"a");

            behaviour->owner->sendMsg(reply);

        }
        break;
    }
    case ProtocolStates::STARTED:{
        // listen for end of protocol msg ---todo implement ---
        VSMMessage* res = behaviour->receive(MessageContents::TPFP_DONE);
        if(res!= 0){
            wasSuccessful = true;
            return true;// end this protocol i.e inform behaviour that protocol is ended

        }
    }
        break;

    default:
        break;
    }
    return false;
}
//returns true if protocol has ended
bool TwoPointFormationProtocol::movingBeaconTick()
{
    BaseProtocol::tick();// to execute querryWithReply
    switch (state) {
    case ProtocolStates::WAITING_REPLY:{
        VSMMessage* res = behaviour->receive(MessageContents::AGREE);
        if(res!= 0){
            stillBeaconId=res->senderNumber;// read other beacon id from its reply, id will be needed in dist measurement
            startDistanceMeasurement(stillBeaconId,ProtocolStates::WAITING_REPLY,ProtocolStates::TIMEOUT);// return to this same state after measurement is received or timeout state if not

        }
        if(latestMeasurement!=0){//measurement result is received
            measuredDist[0]= latestMeasurement;
            enterState(ProtocolStates::FIRST_MOVE);
        }
    }
        break;
    case ProtocolStates::WAITING_DIST_MEASURE_RESULT:{
        measureWaitCounter++;
        VSMMessage* res = behaviour->receive(MessageContents::DISTANCE_MEASUREMENT);
        if(res!= 0){
            latestMeasurement = 10*std::stoi(res->content);//multiplying by ten to switch to mm
            std::cout<<"integer meas. result "<<latestMeasurement<<"\n";
            state = nextStateOnPositeiveResult; //
            measureRetryCounter=0;// reset counter
        }else if(measureWaitCounter>measureResWaitTicks){// if result is not received within timeout(ticks) then retry measurement
            state = ProtocolStates::TIMEOUT;//
            measureWaitCounter=0;
        }
    }
        break;
    case ProtocolStates::FIRST_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeaconId,ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED,ProtocolStates::TIMEOUT);
            std::cout<<">> tpfp started second measurement\n";
        }break;
    case ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED:{// now we have two measurements and traveled distance
        measuredDist[1]=latestMeasurement;
        int dist = behaviour->owner->movementManager->odometry-odoBeforeTravel;
        std::cout<<"distTraveled "<<dist<<"\n";

        relativeAngleH1 = calculateRelativeAngle(measuredDist[0], measuredDist[1], dist);
        relativeAngleH2 = -calculateRelativeAngle(measuredDist[0], measuredDist[1], dist);
        
        std::cout<<"relAngleH1 "<<relativeAngleH1*180/PI<<"  relAngleH2 "<<relativeAngleH2<<"\n";
        
        dirBeforeTurn = behaviour->owner->movementManager->direction;
        behaviour->owner->movementManager->turn(relativeAngleH1*180/PI);//todo turn right is in sim, implement in movement manager

        state = ProtocolStates::TURN_DEGREES;}
        break;
    case ProtocolStates::TURN_DEGREES:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            relativeAngleH1AfterTurn = relativeAngleH1 - PI*(behaviour->owner->movementManager->direction - dirBeforeTurn)/180;// angle for triangle calc
            relativeAngleH2AfterTurn = relativeAngleH2 - PI*(behaviour->owner->movementManager->direction - dirBeforeTurn)/180;// angle for triangle calc
            enterState(ProtocolStates::SECOND_MOVE);// starts movement and changes state
        }
        break;
    case ProtocolStates::SECOND_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeaconId,ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED,ProtocolStates::TIMEOUT);

        }
        break;
    case ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED:
    {
        int dist = behaviour->owner->movementManager->odometry-odoBeforeTravel;
        std::cout<<"distTraveled "<<dist<<"\n";
        measuredDist[2]=latestMeasurement;
        double predictedDistH1 = calcThirdSide(measuredDist[1], dist, relativeAngleH1AfterTurn);
        double predictedDistH2 = calcThirdSide(measuredDist[1], dist, relativeAngleH2AfterTurn);

        std::cout<<" Actual measure :" << measuredDist[2]<<"\n";
        std::cout<<" Predicted measureH1 :" << predictedDistH1<<"\n";
        std::cout<<" Predicted measureH2 :" << predictedDistH2<<"\n";

        double errH1 = abs(predictedDistH1 - measuredDist[2]);
        double errH2 = abs(predictedDistH2 - measuredDist[2]);

          double da, daTwo;
       double angleAfterMovement=0;
        if (errH1 < errH2) {// we turned right hipo, therefore angle should be 0
            da = relativeAngleH1;
        } else{// other hipo was right, calc angle to it
            da = relativeAngleH2;

         angleAfterMovement = PI - calcAngle(measuredDist[2],dist,measuredDist[1]);

        if (cos(da) > 0)//if last angle to other was mor than 180deg then we should now turn to the left and vice versa
            angleAfterMovement = -angleAfterMovement;
}
        std::cout<<"angle to another after move: " << (angleAfterMovement*180/PI)<<"\n";
        //System.out.println("absolute angle2 : " + Math.toDegrees(owner.publicPartOfAgent.direction));

        angleToOtherRobot = angleAfterMovement;


        double turnBy = 0;
        if (measuredDist[2] < BEACONS_TRIANGLE_SIDE_MM) {
            //move away from other robot
            turnBy = angleAfterMovement + PI;
        } else {
            turnBy = angleAfterMovement;
        }
        // System.out.println("turn by: "+Math.toDegrees(turnBy));

        behaviour->owner->movementManager->turn(turnBy*180/PI);//todo turn right is in sim, implement in movement manager

        state = ProtocolStates::FINAL_POSITION_TURN;


    }
        break;

    case ProtocolStates::FINAL_POSITION_TURN:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done

            double distToTravel = abs(measuredDist[2] - BEACONS_TRIANGLE_SIDE_MM);
            behaviour->owner->movementManager->driveDistance(distToTravel);  // drive forward distace

            enterState(ProtocolStates::FINAL_POSITION_MOVE);
        }


        break;
    case ProtocolStates::FINAL_POSITION_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeaconId,ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED,ProtocolStates::TIMEOUT);
        }
        break;
    case ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED:{
        cout<<"final check measurement: "<<latestMeasurement<<"\n";
        if(acknowledgeRetryCounter++ >=finalAcknowledgeRetries)
        {
            wasSuccessful = false;
            return false;
            break;
        }
        VSMMessage tpfpFinished(behaviour->owner->id,Topics::THIRD_BEACON_IN,MessageContents::TPFP_DONE,"a");

        querryWithTimeout(tpfpFinished,MessageContents::ACKNOWLEDGE,ProtocolStates::ACKNOWLEDGE_RECEIVED,ProtocolStates::TIMEOUT,3,3);

       // behaviour->owner->sendMsg(tpfpFinished);
       // state = ProtocolStates::WAITING_ACKNOWLEDGE;
    }
        break;
    case ProtocolStates::WAITING_ACKNOWLEDGE:{// todo erease because it is handled in superclass
        if(acknowledgeWaitCounter++ >finalAcknowledgeWaitTicks){// resend finished message to third beacon
            state= ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED;
        }
        VSMMessage* res = behaviour->receive(MessageContents::ACKNOWLEDGE);
        if(res!= 0){
            state = ProtocolStates::ACKNOWLEDGE_RECEIVED;
        }}
        break;
    case ProtocolStates::ACKNOWLEDGE_RECEIVED:
    {
        wasSuccessful = true;// indicates that owner behaviour of this protocol can proceed with next protocol
        return true;
    }
        break;

    case ProtocolStates::TIMEOUT:{
        measureRetryCounter++;
        std::cout<<"tpfp 1 timeout "<<measureRetryCounter<<"\n";
        if(measureRetryCounter<=measureRetries){
            startDistanceMeasurement(stillBeaconId,nextStateOnPositeiveResult,nextStateOnNegativeResult);
            break;
        }
        std::cout<<"tpfp failed to measure dist, stopping";
        wasSuccessful = false;
        return true;
    }   break;
    }
    return false;
}

void TwoPointFormationProtocol::enterState(ProtocolStates stateToEnter)
{
    switch (stateToEnter) {

    case ProtocolStates::FIRST_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        odoBeforeTravel = behaviour->owner->movementManager->odometry;
        behaviour->owner->movementManager->driveDistance(TRIANGLE_SIDE_MM);  // drive forward distace
        state = ProtocolStates::FIRST_MOVE;
        cout<<("entering first move\n");
        break;

    case ProtocolStates::SECOND_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        odoBeforeTravel = behaviour->owner->movementManager->odometry;
        behaviour->owner->movementManager->driveDistance(TRIANGLE_SIDE_MM/2);  // drive forward distace
        state = ProtocolStates::SECOND_MOVE;
        cout<<("entering second move\n");
        break;

    case ProtocolStates::FINAL_POSITION_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        odoBeforeTravel = behaviour->owner->movementManager->odometry;
        state = ProtocolStates::FINAL_POSITION_MOVE;
        cout<<("entering final move\n");
        break;
    }
}

void TwoPointFormationProtocol::startDistanceMeasurement(int id,ProtocolStates nextStatePosResult,ProtocolStates nextStateNegResult)
{
    latestMeasurement=0;
    nextStateOnNegativeResult = nextStateNegResult;// assign these values directly prior this call?
    nextStateOnPositeiveResult = nextStatePosResult;
    behaviour->owner->uwbMsgListener.addToRangingInitDeque(id);//send measurement initation
    state = ProtocolStates::WAITING_DIST_MEASURE_RESULT;
}


TwoPointFormationProtocol::TwoPointFormationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{
    this->roleInProtocol=roleInProtocol;
    if(roleInProtocol==RoleInProtocol::STANDING_BEACON){// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::TWO_POINT_FORMATION_TO_STILL);
        behaviour->subscribeToTopic(Topics::THIRD_BEACON_IN);// to receive tpfp done from moving beacon
    }
    else{
        behaviour->subscribeToTopic(Topics::TWO_POINT_FORMATION_TO_MOVING);
    }
    behaviour->subscribeToDirectMsgs();
}



double TwoPointFormationProtocol::calculateRelativeAngle(double mes1, double mes2, double odoDist)
{
    if ((mes2 + odoDist) < mes1) return 0;
    if ((mes1 + odoDist) < mes2) return PI;// todo revisit
    double angle = acos((odoDist * odoDist + mes2 * mes2 - mes1 * mes1) / (2 * odoDist * mes2));
    std::cout<<"calcRelAngle called "<<mes1<<" "<<mes2<<" "<<odoDist<<" angle: "<<angle<<"\n";

    return PI - angle;
}
double TwoPointFormationProtocol::calcThirdSide(double a,double b,double angleRad){
       std::cout<<"calc3rdSide called "<<a<<" "<<b<<" "<<angleRad*180/PI<<"\n";

    return sqrt(a*a+b*b-2*a*b*cos(angleRad));
}

double TwoPointFormationProtocol::calcAngle(double a, double b, double c)
{ std::cout<<"calcAngle called "<<a<<" "<<b<<" "<<c<<"\n";
    if ((a + b) < c) return PI;
    if (abs(a-b) > c) return 0;
    //cos theorem

    return acos((a*a+b*b-c*c)/(2*a*b));


}
