#include "thirdBeaconFormationProtocol.hpp"
#include "twoPointFormationProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>

#define TRIANGLE_SIDE_MM 1000
#define BEACONS_TRIANGLE_SIDE_MM 1500

#define PI 3.1415926
bool ThirdBeaconFormationProtocol::tick()
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

void ThirdBeaconFormationProtocol::start()
{
    switch (roleInProtocol) {
    case RoleInProtocol::STANDING_BEACON:// send query for confirmation of protocol start to still beacon
        state = ProtocolStates::WAITING_START_REQUEST;
        break;
    case RoleInProtocol::MOVING_BEACON:{
        VSMMessage startRequest(behaviour->owner->id,Topics::THIRD_BEACON_OUT,MessageContents::TPF_START,"r");
        behaviour->owner->sendMsg(startRequest);
        state = ProtocolStates::WAITING_REPLY;
    }
    default:
        break;
    }
}


bool ThirdBeaconFormationProtocol::standingBeaconTick()
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
    case ProtocolStates::STARTED:
        // listen for end of protocol msg ---todo implement ---
        break;

    default:
        break;
    }
    return false;
}

//returns true if protocol has ended
bool ThirdBeaconFormationProtocol::movingBeaconTick()
{
    switch (state) {
    case ProtocolStates::WAITING_REPLY:{
        VSMMessage* res = behaviour->receive(MessageContents::AGREE);
        if(res!= 0){
           stillBeaconResponders.insert(res->senderNumber);// read other beacon id from its reply, id will be needed in dist measurement
           if(stillBeaconResponders.size()>=2){
               stillBeacon1Id = *(stillBeaconResponders.begin());
               stillBeacon2Id = *(stillBeaconResponders.begin()+1);
               startDistanceMeasurement(stillBeacon1Id,ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_1,ProtocolStates::TIMEOUT);// return to this same state after measurement is received or timeout state if not

           }

        }

    }
        break;

    case ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_1:
         measuredDistToFirst[0]= latestMeasurement;//
         startDistanceMeasurement(stillBeacon2Id,ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_2,ProtocolStates::TIMEOUT);// return to this same state after measurement is received or timeout state if not

        break;
    case ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_2:
         measuredDistToSecond[0]= latestMeasurement;//
enterState(ProtocolStates::FIRST_MOVE);
        break;

    case ProtocolStates::WAITING_DIST_MEASURE_RESULT:{
        measureWaitCounter++;
        VSMMessage* res = behaviour->receive(MessageContents::DISTANCE_MEASUREMENT);
        if(res!= 0){
            latestMeasurement = 10*std::stoi(res->content);//multiplying by ten to switch to mm
            std::cout<<"integer meas. result "<<latestMeasurement<<"\n";
            state = nextStateOnPositeiveResult; //
        }else if(measureWaitCounter>measureResWaitTicks){// if result is not received within timeout(ticks) then retry measurement
            state = ProtocolStates::TIMEOUT;//
            measureWaitCounter=0;
        }
    }
        break;
    case ProtocolStates::FIRST_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeacon1Id,ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED,ProtocolStates::TIMEOUT);
            std::cout<<">> 3rdbfp started second measurement\n";
        }break;
    case ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED:// now we have two measurements and traveled distance
        measuredDistToFirst[1]=latestMeasurement;
        relativeAngleH1 = calculateRelativeAngle(measuredDist[0], measuredDist[1], TRIANGLE_SIDE_MM);
        relativeAngleH2 = -calculateRelativeAngle(measuredDist[0], measuredDist[1], TRIANGLE_SIDE_MM);
        
        std::cout<<"relAngleH1 "<<relativeAngleH1<<"  relAngleH2 "<<relativeAngleH2<<"\n";
       // todo -continue from here- measure dist to second beacon
        dirBeforeTurn = behaviour->owner->movementManager->direction;
        behaviour->owner->movementManager->turn(relativeAngleH1*180/PI);//todo turn right is in sim, implement in movement manager

        state = ProtocolStates::TURN_DEGREES;
        break;
    case ProtocolStates::TURN_DEGREES:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            relativeAngleH1 = relativeAngleH1 - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc
            relativeAngleH2 = relativeAngleH2 - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc
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
        measuredDist[2]=latestMeasurement;
        double predictedDistH1 = calcThirdSide(measuredDist[1], TRIANGLE_SIDE_MM / 2, relativeAngleH1);
        double predictedDistH2 = calcThirdSide(measuredDist[1], TRIANGLE_SIDE_MM / 2, relativeAngleH2);

        std::cout<<" Actual measure :" << measuredDist[2]<<"\n";
        std::cout<<" Predicted measureH1 :" << predictedDistH1<<"\n";
        std::cout<<" Predicted measureH2 :" << predictedDistH2<<"\n";

        double errH1 = abs(predictedDistH1 - measuredDist[2]);
        double errH2 = abs(predictedDistH2 - measuredDist[2]);

        double da, daTwo;
        if (errH1 < errH2) {
            da = relativeAngleH1;
        } else
            da = relativeAngleH2;
        // System.out.println("da : " + Math.toDegrees(da));

        double angleAfterMovement = PI - calcAngle(measuredDist[2],TRIANGLE_SIDE_MM/2,measuredDist[1]);

        if (sin(da) > 0)//if last angle to other was mor than 180deg then we should now turn to the left and vice versa
            angleAfterMovement = -angleAfterMovement;

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
    case ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED:
        cout<<"final check measurement: "<<latestMeasurement<<"\n";
        //todo - how to end protocol?
        //sendReadyToMaster();
        //enterState(FormationBehaviourStates.WAITING_BEAC_MASTER_REQUEST);
        wasSuccessful = true;// indicates that owner behaviour of this protocol can proceed with next protocol
        return true;
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

void ThirdBeaconFormationProtocol::enterState(ProtocolStates stateToEnter)
{
    switch (stateToEnter) {

    case ProtocolStates::FIRST_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        behaviour->owner->movementManager->driveDistance(TRIANGLE_SIDE_MM);  // drive forward distace
        state = ProtocolStates::FIRST_MOVE;
        cout<<("3rdbfp entering first move\n");
        break;

    case ProtocolStates::SECOND_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        behaviour->owner->movementManager->driveDistance(TRIANGLE_SIDE_MM/2);  // drive forward distace
        state = ProtocolStates::SECOND_MOVE;
        cout<<("3rdbfp entering second move\n");
        break;

    case ProtocolStates::FINAL_POSITION_MOVE:
        // start to move robot: owner.publicPartOfAgent.moveForwardBy(triangleTravelside);
        state = ProtocolStates::FINAL_POSITION_MOVE;
        cout<<("3rdbfp entering final move\n");
        break;
    }
}

void ThirdBeaconFormationProtocol::startDistanceMeasurement(int id,ProtocolStates nextStatePosResult,ProtocolStates nextStateNegResult)
{
    latestMeasurement=0;
    nextStateOnNegativeResult = nextStateNegResult;// assign these values directly prior this call?
    nextStateOnPositeiveResult = nextStatePosResult;
    behaviour->owner->uwbMsgListener.addToRangingInitDeque(id);//send measurement initation
    state = ProtocolStates::WAITING_DIST_MEASURE_RESULT;
}


ThirdBeaconFormationProtocol::ThirdBeaconFormationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{
    this->roleInProtocol=roleInProtocol;
    if(roleInProtocol==RoleInProtocol::STANDING_BEACON)// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::TWO_POINT_FORMATION_TO_STILL);
    else{
        behaviour->subscribeToTopic(Topics::TWO_POINT_FORMATION_TO_MOVING);
    }
    behaviour->subscribeToDirectMsgs();
}



