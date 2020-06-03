#include "operationsManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>
#include "s2BeaconsBehaviour.hpp"
#include "beaconManagementProtocol.hpp"
#include "s2ExplorersBehaviour.hpp"
#include "s4Behaviour.hpp"
//#include "baseProtocol.hpp"

#define REPLY_WAITING_TICKS 5/TICK_PERIOD_SEC
#define BEACONS_COUNT_NORMAL 3
#define BEACONS_COUNT_REFORMATION 6

OperationsManagementProtocol::OperationsManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{
    this->roleInProtocol=roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S3)// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::S3_IN);
    else if (roleInProtocol==RoleInProtocol::S2BEACON) {
        behaviour->subscribeToTopic(Topics::S2BEACONS_IN);
    }
    else if (roleInProtocol==RoleInProtocol::S2EXPLORERS) {
        behaviour->subscribeToTopic(Topics::S2EXPLORERS_IN);
    }
    else if (roleInProtocol==RoleInProtocol::S4) {
        behaviour->subscribeToTopic(Topics::TO_S4);
    }

    behaviour->subscribeToDirectMsgs();
}

void OperationsManagementProtocol::start()
{
    switch (roleInProtocol) {
    case RoleInProtocol::S3:{
        //send request to S2Beacons to start first formation and wait for reply

        VSMMessage startRequest(behaviour->owner->id,Topics::S2BEACONS_IN,MessageContents::FIRST_FORMATION_START,"r");
        behaviour->owner->sendMsg(startRequest);
        enterState(ProtocolStates::WAITING_REPLY);
        //  cout<<"omp s3 sent start formation to s2b\n";
    }
        break;
    case RoleInProtocol::S2BEACON:
        state = ProtocolStates::WAITING_START_REQUEST;
        break;
    case RoleInProtocol::S2EXPLORERS:
        state = ProtocolStates::WAITING_START_REQUEST;// does not matter for test
        break;
    case RoleInProtocol::S4:
        state = ProtocolStates::IDLE;//dont care of initial state, protocol will start when initiateRegroup() will be called
        break;
    }

}

bool OperationsManagementProtocol::tick(){

    switch (roleInProtocol) {
    case RoleInProtocol::S3:
        return s3Tick();
        break;
    case RoleInProtocol::S2BEACON:
        return s2BeaconsTick();
        break;
    case RoleInProtocol::S2EXPLORERS:
        return s2ExplorersTick();
        break;
    case RoleInProtocol::S4:
        return s4Tick();
        break;

    default:
        break;
    }
}

void OperationsManagementProtocol::initiateBeaconsRegroup()//for s4
{
    VSMMessage regroupRequestToS3(VSMSubsystems::S4,Topics::S3_IN,MessageContents::EXPLORING_DONE,"ed");
    querryWithTimeout(regroupRequestToS3,MessageContents::ACKNOWLEDGE,ProtocolStates::ACKNOWLEDGE_RECEIVED,ProtocolStates::TIMEOUT,3,3);//receive reply and inform s2beaconsof new cordinates
}

bool OperationsManagementProtocol::s4Tick(){
    BaseProtocol::tick();
    switch (state) {
    case ProtocolStates::BEACONS_DEPLOYED:
        //receive visited points from bm

        break;
    case ProtocolStates::ACKNOWLEDGE_RECEIVED:{
        vector<int> cords = ((S4Behaviour*)behaviour)->getCordsForRegrouping(BEACONS_TRIANGLE_SIDE_MM/10);
        string s = BaseProtocol::intVectorToString(cords);
        VSMMessage regroupRequestToS2Beacons(VSMSubsystems::S4,Topics::S2BEACONS_IN,MessageContents::EXPLORING_DONE,s);
        querryWithTimeout(regroupRequestToS2Beacons,MessageContents::ACKNOWLEDGE,ProtocolStates::REGROUPING,ProtocolStates::TIMEOUT,3,3);//receive reply and inform s2beaconsof new cordinates

    }   break;

    case ProtocolStates::REGROUPING:{


    }break;

    default:
        break;
    }
    //
    return false;
}

bool OperationsManagementProtocol::s3Tick()// todo - use protocol state or s3 behavior state?
{
    //  cout<<"omp s3 tick state "<<(int)state <<"\n";

    switch (state) {
    case ProtocolStates::WAITING_REPLY:
    {

        VSMMessage* res = behaviour->receive(MessageContents::AGREE);
        if(res!= 0){
            //enter state waiting formation complete
            enterState(ProtocolStates::WAITING_FORMATION_COMPLETE);
            delete res;
            break;
        }
        // std::cout<<"s3 omp waitingTicksCounter"<<waitTicksCounter<<"\n";

        waitTicksCounter++;
        if(waitTicksCounter>=REPLY_WAITING_TICKS){
            waitTicksCounter=0;
            start();// start again with request if no confirmation is received within timeout
        }
    }
        break;
    case ProtocolStates::WAITING_FORMATION_COMPLETE:{
        VSMMessage* res = behaviour->receive(MessageContents::FORMATION_COMPLETED);
        if(res!= 0){
            //enter next state - todo
            std::cout<<"omp master(s3) received formation complete\n";
            enterState(ProtocolStates::BEACONS_DEPLOYED);
            delete res;
        }
    }
        break;

    case ProtocolStates::BEACONS_DEPLOYED:{//stay in this state until we are ready to continue with next operation- move beacons to new place
        VSMMessage* res = behaviour->receive(MessageContents::EXPLORING_DONE);
        if(res!= 0){// s4 sent that exploring is done, increase cValues of beacons so that they can regroup
            std::cout<<"s3 received exploring done,update cValues\n";
            //reply to sender
            VSMMessage agree(VSMSubsystems::S3,Topics::TO_S4,MessageContents::ACKNOWLEDGE,"ac");
            behaviour->owner->sendMsg(agree);
            enterState(ProtocolStates::REGROUPING);
        }

    }  break;
    case ProtocolStates::REGROUPING:{
        //if s4 did not receive initial reply it will continue to querry, so listen also now for those msgs
        VSMMessage* res = behaviour->receive(MessageContents::EXPLORING_DONE);
        if(res!= 0){// s4 sent that exploring is done, increase cValues of beacons so that they can regroup

            //reply to sender
            VSMMessage agree(VSMSubsystems::S3,Topics::TO_S4,MessageContents::ACKNOWLEDGE,"ac");
            behaviour->owner->sendMsg(agree);
        }



    }break;
    }
    return false;
}



bool OperationsManagementProtocol::s2BeaconsTick()
{
    switch (state) {
    case ProtocolStates::WAITING_START_REQUEST:{
        VSMMessage* res = behaviour->receive(MessageContents::FIRST_FORMATION_START);
        if(res!= 0){
            //owner s2 should send beacon info queries to see if there is enough beacons, choose all three beacons and make first of them to start tpfp

            //send reply to s3, but is this confirmation really needed?
            VSMMessage agree(behaviour->owner->id,Topics::S3_IN,MessageContents::AGREE,"ag");
            behaviour->owner->sendMsg(agree);

            cout<<"S2 beacons received -start first formation \n";
            ((BeaconManagementProtocol*) (((S2BeaconsBehaviour*)behaviour)->s1ManagementProtocol))->start();
            cout<<"omp S2 beacons start returned \n";

            //send confirmation to s3 - todo
            state = ProtocolStates::WAITING_FORMATION_COMPLETE;// same state as s3
            delete res;
        }}
        break;
    case ProtocolStates::WAITING_FORMATION_COMPLETE:{// todo move this to aprppriate state e.g. exploring
        VSMMessage* res = behaviour->receive(MessageContents::EXPLORING_DONE);
        if(res!= 0){// s4 sent that exploring is done, find additional 3 beacons to send them to the new positions
            std::cout<<"s2b received exploring done,regrouping \n";
vector<int>cords = BaseProtocol::stringTointVector(res->content);
            ((S2BeaconsBehaviour*)behaviour)->startRegroupingBeacons(cords);
            //reply to sender
            VSMMessage agree(VSMSubsystems::S2_BEACONS,Topics::TO_S4,MessageContents::ACKNOWLEDGE,"acb");
            behaviour->owner->sendMsg(agree);
       state = ProtocolStates::REGROUPING;

        }
    }
        break;
    case ProtocolStates::REGROUPING:{



    }break;

    }
    return false;
}

bool OperationsManagementProtocol::s2ExplorersTick()
{
    //receive start exploring command and propogate it to s1 as they joins explorers
    VSMMessage* res = behaviour->receive(MessageContents::START_EXPLORING);
    if(res!= 0){
        std::cout<<"explorers S2 received start exploring \n";
        ((S2ExplorersBehaviour*)behaviour)->enterExploringState();
        delete res;
    }
    VSMMessage* res2 = behaviour->receive(MessageContents::STOP_EXPLORING);
    if(res2!= 0){
        std::cout<<"explorers S2 received stop exploring \n";
        ((S2ExplorersBehaviour*)behaviour)->enterIdleState();
    }
}

void OperationsManagementProtocol::enterState(ProtocolStates stateToEnter)// for s3
{
    switch (stateToEnter) {
    case ProtocolStates::WAITING_REPLY:
    {
        std::cout<<"s3 omp entering state WAITING REPLY\n";

        state = ProtocolStates::WAITING_REPLY;
        ((S3Behaviour*)behaviour)->updateCvals(BEACONS_COUNT_NORMAL);

    }
        break;

    case ProtocolStates::WAITING_FORMATION_COMPLETE:{
        std::cout<<"s3 omp entering state WAITING FORMATION COMPLETE\n";
        state = ProtocolStates::WAITING_FORMATION_COMPLETE;
        ((S3Behaviour*)behaviour)->updateCvals(BEACONS_COUNT_NORMAL);
    }
        break;

    case ProtocolStates::BEACONS_DEPLOYED:{
        std::cout<<"s3 omp entering state BEACONS DEPLOYED\n";

        state = ProtocolStates::BEACONS_DEPLOYED;
        ((S3Behaviour*)behaviour)->updateCvals(BEACONS_COUNT_NORMAL);// todo for test deceresd val by 1
        //send explorers s2 to start exploring
        VSMMessage startRequest(behaviour->owner->id,Topics::S2EXPLORERS_IN,MessageContents::START_EXPLORING,"se");// wait confirmation?
        behaviour->owner->sendMsg(startRequest);
    }
        break;
    case ProtocolStates::REGROUPING:{
        std::cout<<"s3 omp entering state REGROUPING\n";
        state = ProtocolStates::REGROUPING;
        ((S3Behaviour*)behaviour)->updateCvals(BEACONS_COUNT_REFORMATION);
    }
        break;
    }
}
