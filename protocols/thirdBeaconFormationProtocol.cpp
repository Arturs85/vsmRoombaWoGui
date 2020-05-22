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
    case RoleInProtocol::STANDING_BEACON:{// send query for confirmation of protocol start to still beacon
        //        VSMMessage startRequest(behaviour->owner->id,Topics::THIRD_BEACON_IN,MessageContents::TPFP_DONE,"r");// tell 3rd beacon that still beacon is ready
        //        behaviour->owner->sendMsg(startRequest);
        state = ProtocolStates::WAITING_START_REQUEST;
    }
        break;
    case RoleInProtocol::MOVING_BEACON:{


        VSMMessage startRequest(behaviour->owner->id,Topics::THIRD_BEACON_OUT,MessageContents::THIRD_BFP_START,"r");
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
    case ProtocolStates::WAITING_START_REQUEST:{ // add timeout to resend tpfp done if no acknowledgement received from 3rd beacon
        VSMMessage* res = behaviour->receive(MessageContents::THIRD_BFP_START);
        if(res!= 0){
            state= ProtocolStates::STARTED;
            VSMMessage reply(behaviour->owner->id,Topics::THIRD_BEACON_IN,MessageContents::AGREE,"a");

            behaviour->owner->sendMsg(reply);

        }
        break;
    }
    case ProtocolStates::STARTED:{
        // listen for end of protocol msg ---todo implement ---
        VSMMessage* res = behaviour->receive(MessageContents::FORMATION_COMPLETED);
        if(res!= 0){
            wasSuccessful = true;
            state = ProtocolStates::FINISHED;
            return true;
        }
    }
        break;
    case ProtocolStates::FINISHED:{
        return true;
    }
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
    case ProtocolStates::IDLE:
    {
        VSMMessage* res = behaviour->receive(MessageContents::TPFP_DONE);
        if(res!= 0){
            VSMMessage acknowledge(behaviour->owner->id,Topics::TWO_POINT_FORMATION_TO_MOVING,MessageContents::ACKNOWLEDGE,"r");
            behaviour->owner->sendMsg(acknowledge);// this msg will allow tpfp moving beacon to end its protocol
            start();
        }
    }
        break;
    case ProtocolStates::WAITING_REPLY:{// wait messages from
        VSMMessage* res = behaviour->receive(MessageContents::AGREE);
        if(res!= 0){
            stillBeaconResponders.insert(res->senderNumber);// read other beacon id from its reply, id will be needed in dist measurement
            if(stillBeaconResponders.size()>=2){
                stillBeacon1Id = *(stillBeaconResponders.begin());
                stillBeacon2Id = *(++(stillBeaconResponders.begin()));
                startDistanceMeasurement(stillBeacon1Id,ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_1,ProtocolStates::TIMEOUT);// return to this same state after measurement is received or timeout state if not

            }
            replyWaitCounter=0;
        }
        if(replyWaitCounter++>replyResWaitTicks){
            if(queryRetryCounter++ > queryRetries){
                wasSuccessful = false;
                return true;
            }
            start();// resend invitations
        }

    }
        break;

    case ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_1:
        std::cout<<"3rdbfp first measurement received from 1";
        measuredDistToFirst[0]= latestMeasurement;//
        startDistanceMeasurement(stillBeacon2Id,ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_2,ProtocolStates::TIMEOUT);// return to this same state after measurement is received or timeout state if not

        break;
    case ProtocolStates::FIRST_MEASUREMENT_RECEIVED_FROM_2:
        std::cout<<"3rdbfp first measurement received from 2";

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
        relativeAngleH1First = TwoPointFormationProtocol::calculateRelativeAngle(measuredDistToFirst[0], measuredDistToFirst[1], TRIANGLE_SIDE_MM);
        relativeAngleH2First = -TwoPointFormationProtocol::calculateRelativeAngle(measuredDistToFirst[0], measuredDistToFirst[1], TRIANGLE_SIDE_MM);
        
        std::cout<<"relAngleH1f "<<relativeAngleH1First<<"  relAngleH2f "<<relativeAngleH2First<<"\n";
        startDistanceMeasurement(stillBeacon2Id,ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED_FROM_2,ProtocolStates::TIMEOUT);

        break;
    case ProtocolStates::FIRST_MOVE_MEASUREMENT_RECEIVED_FROM_2:
        measuredDistToSecond[1] = latestMeasurement;
        relativeAngleH1Second = TwoPointFormationProtocol::calculateRelativeAngle(measuredDistToSecond[0], measuredDistToSecond[1], TRIANGLE_SIDE_MM);
        relativeAngleH2Second = -TwoPointFormationProtocol::calculateRelativeAngle(measuredDistToSecond[0], measuredDistToSecond[1], TRIANGLE_SIDE_MM);

        dirBeforeTurn = behaviour->owner->movementManager->direction;
        behaviour->owner->movementManager->turn(relativeAngleH1First*180/PI);//todo turn right is in sim, implement in movement manager

        state = ProtocolStates::TURN_DEGREES;
        break;
    case ProtocolStates::TURN_DEGREES:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            relativeAngleH1First = relativeAngleH1First - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc
            relativeAngleH2First = relativeAngleH2First - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc

            relativeAngleH1Second = relativeAngleH1Second - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc
            relativeAngleH2Second = relativeAngleH2Second - (behaviour->owner->movementManager->direction - dirBeforeTurn);// angle for triangle calc


            enterState(ProtocolStates::SECOND_MOVE);// starts movement and changes state
        }
        break;
    case ProtocolStates::SECOND_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeacon1Id,ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED,ProtocolStates::TIMEOUT);

        }
        break;
    case ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED:
        measuredDistToFirst[2]=latestMeasurement;
        startDistanceMeasurement(stillBeacon2Id,ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED_FROM_2,ProtocolStates::TIMEOUT);
        break;

    case ProtocolStates::SECOND_MOVE_MEASUREMENT_RECEIVED_FROM_2:
    {
        measuredDistToSecond[2]=latestMeasurement;

        double predictedDistH1First = TwoPointFormationProtocol::calcThirdSide(measuredDistToFirst[1], TRIANGLE_SIDE_MM / 2, relativeAngleH1First);
        double predictedDistH2First = TwoPointFormationProtocol::calcThirdSide(measuredDistToFirst[1], TRIANGLE_SIDE_MM / 2, relativeAngleH2First);
        double predictedDistH1Second = TwoPointFormationProtocol::calcThirdSide(measuredDistToSecond[1], TRIANGLE_SIDE_MM / 2, relativeAngleH1Second);
        double predictedDistH2Second = TwoPointFormationProtocol::calcThirdSide(measuredDistToSecond[1], TRIANGLE_SIDE_MM / 2, relativeAngleH2Second);



        std::cout<<" Actual measure :" << measuredDistToFirst[2]<<"\n";
        std::cout<<" Predicted measureH1 :" << predictedDistH1First<<"\n";
        std::cout<<" Predicted measureH2 :" << predictedDistH2First<<"\n";
        std::cout<<" Predicted measureH1 to second :" << predictedDistH1Second<<"\n";
        std::cout<<" Predicted measureH2 to second :" << predictedDistH2Second<<"\n";
        
        double errH1 = abs(predictedDistH1First - measuredDistToFirst[2]);
        double errH2 = abs(predictedDistH2First - measuredDistToFirst[2]);
        double errH1Two = abs(predictedDistH1Second - measuredDistToSecond[2]);
        double errH2Two = abs(predictedDistH2Second - measuredDistToSecond[2]);

        double da, daTwo;
        if (errH1 < errH2) {
            da = relativeAngleH1First;
        } else
            da = relativeAngleH2First;
        // System.out.println("da : " + Math.toDegrees(da));
        if (errH1Two < errH2Two) {
            daTwo = relativeAngleH1Second;
        } else
            daTwo = relativeAngleH2Second;

        double angleAfterMovement = PI - TwoPointFormationProtocol::calcAngle(measuredDistToFirst[2],TRIANGLE_SIDE_MM/2,measuredDistToFirst[1]);
        double angleAfterMovementSecond = PI - TwoPointFormationProtocol::calcAngle(measuredDistToSecond[2],TRIANGLE_SIDE_MM/2,measuredDistToSecond[1]);

        if (sin(da) > 0)//if last angle to other was mor than 180deg then we should now turn to the left and vice versa
            angleAfterMovement = -angleAfterMovement;
        if (sin(daTwo) > 0)//if last angle to other was mor than 180deg then we should now turn to the left and vice versa
            angleAfterMovementSecond = -angleAfterMovementSecond;

        std::cout<<"angle to another b1 after move: " << (angleAfterMovement*180/PI)<<"\n";
        std::cout<<"angle to another b2 after move: " << (angleAfterMovementSecond*180/PI)<<"\n";
        
        //System.out.println("absolute angle2 : " + Math.toDegrees(owner.publicPartOfAgent.direction));

        angleToFirstRobot = angleAfterMovement;
        angleToSecondRobot = angleAfterMovementSecond;
        // calc angle to turn to reach third vertex of beacons triangle

        double closestVertexDist = std::min(measuredDistToFirst[2], measuredDistToSecond[2]);
        double apb = TwoPointFormationProtocol::calcAngle(closestVertexDist, BEACONS_TRIANGLE_SIDE_MM, std::max(measuredDistToFirst[2], measuredDistToSecond[2]));// warning - using nominal not actual triangle side
        // System.out.println("apb : " + Math.toDegrees(apb));
        double alfa = apb - PI / 3; //todo check this final angle calculation method
        finalDistance = TwoPointFormationProtocol::calcThirdSide(closestVertexDist, BEACONS_TRIANGLE_SIDE_MM, alfa);
        finalAngle = TwoPointFormationProtocol::calcAngle(finalDistance, closestVertexDist, BEACONS_TRIANGLE_SIDE_MM);
        std::cout<<"final angle: " << (angleAfterMovement*180/PI)<<"\n";
        
        // System.out.println("final angle : " + Math.toDegrees(finalAngle));
        dirBeforeTurn = behaviour->owner->movementManager->direction;

        behaviour->owner->movementManager->turn(finalAngle*180/PI);

        state = ProtocolStates::FINAL_POSITION_TURN;


    }
        break;

    case ProtocolStates::FINAL_POSITION_TURN:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done

            behaviour->owner->movementManager->driveDistance(finalDistance);  // drive forward distace
            odoBeforeTravel = behaviour->owner->movementManager->odometry;
            enterState(ProtocolStates::FINAL_POSITION_MOVE);
        }


        break;
    case ProtocolStates::FINAL_POSITION_MOVE:
        if(behaviour->owner->movementManager->state==MovementStates::FINISHED){// movement is done
            startDistanceMeasurement(stillBeacon1Id,ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED,ProtocolStates::TIMEOUT);
        }
        break;
    case ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED:
        measuredDistToFirst[3]= latestMeasurement;
        startDistanceMeasurement(stillBeacon2Id,ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED_FROM_2,ProtocolStates::TIMEOUT);
        break;
    case ProtocolStates::FINAL_POSITION_MOVE_MEAS_RECEIVED_FROM_2:
    {
        measuredDistToSecond[3]=latestMeasurement;

        cout<<"final check measurement: "<<latestMeasurement<<"\n";
        //sendReadyToMaster();
        //enterState(FormationBehaviourStates.WAITING_BEAC_MASTER_REQUEST);

        // calculate positions of other beacons after final move
        double deltaAngle = behaviour->owner->movementManager->direction - dirBeforeTurn;
        double travel = behaviour->owner->movementManager->odometry-odoBeforeTravel;
        angleToFirstRobot = calcAngleAfterTurnAndMove(angleToFirstRobot,deltaAngle,travel,measuredDistToFirst[3],measuredDistToFirst[2]);

        angleToSecondRobot = calcAngleAfterTurnAndMove(angleToSecondRobot,deltaAngle,travel,measuredDistToFirst[3],measuredDistToSecond[2]);
        //todo - how to end protocol?

        //just send to s2 that formation is done, for now dont wait for answer
        VSMMessage formDone(behaviour->owner->id,Topics::S2BEACONS_IN,MessageContents::FORMATION_COMPLETED,"fc1");
        behaviour->owner->sendMsg(formDone);
        VSMMessage formDone2(behaviour->owner->id,Topics::THIRD_BEACON_OUT,MessageContents::FORMATION_COMPLETED,"fcb");
        behaviour->owner->sendMsg(formDone2);

        state = ProtocolStates::IDLE;//temporary- to do nothing, actually we should not to call this protocol tick() anymore at all
        wasSuccessful = true;// indicates that owner behaviour of this protocol can proceed with next protocol
        return true;
        break;
    }
    case ProtocolStates::TIMEOUT:{
        measureRetryCounter++;
        std::cout<<"3rdbfp 1 timeout "<<measureRetryCounter<<"\n";
        if(measureRetryCounter<=measureRetries){
            startDistanceMeasurement(latestMeasuredBeaconId,nextStateOnPositeiveResult,nextStateOnNegativeResult);
            break;
        }
        std::cout<<"3rdbfp failed to measure dist, stopping";
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
    latestMeasuredBeaconId=id;// to use if retry of measurement is needed
    nextStateOnNegativeResult = nextStateNegResult;// assign these values directly prior this call?
    nextStateOnPositeiveResult = nextStatePosResult;
    behaviour->owner->uwbMsgListener.addToRangingInitDeque(id);//send measurement initation
    state = ProtocolStates::WAITING_DIST_MEASURE_RESULT;
}


ThirdBeaconFormationProtocol::ThirdBeaconFormationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{
    this->roleInProtocol=roleInProtocol;
    if(roleInProtocol==RoleInProtocol::STANDING_BEACON)// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::THIRD_BEACON_OUT);
    else{
        behaviour->subscribeToTopic(Topics::THIRD_BEACON_IN);
        state = ProtocolStates::IDLE;
    }
    behaviour->subscribeToDirectMsgs();
}

double ThirdBeaconFormationProtocol::calcAngleAfterTurnAndMove(double prevRelatAngle, double turnAngle, double travel, double measAfter,double measBefore){
    double angleAfterMovement = PI - TwoPointFormationProtocol::calcAngle(travel,measAfter,measBefore);

    if (sin(prevRelatAngle-turnAngle) < 0)// todo is this right for all cases
        angleAfterMovement = -angleAfterMovement;
    return angleAfterMovement;
}



