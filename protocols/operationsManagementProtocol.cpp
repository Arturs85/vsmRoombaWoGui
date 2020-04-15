#include "operationsManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>
#include "s2BeaconsBehaviour.hpp"
#include "beaconManagementProtocol.hpp"

#define REPLY_WAITING_TICKS 5/TICK_PERIOD_SEC

OperationsManagementProtocol::OperationsManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{
    this->roleInProtocol=roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S3)// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::S3_IN);
    else if (roleInProtocol==RoleInProtocol::S2BEACON) {
        behaviour->subscribeToTopic(Topics::S2BEACONS_IN);
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
        state = ProtocolStates::WAITING_REPLY;
    cout<<"omp s3 sent start formation to s2b\n";
    }
        break;
    case RoleInProtocol::S2BEACON:
        state = ProtocolStates::WAITING_START_REQUEST;
        break;
    }
}

bool OperationsManagementProtocol::tick(){
    switch (roleInProtocol) {
    case RoleInProtocol::S3:
        return s3Tick();
        break;
    case RoleInProtocol::S2BEACON:
        return s2Tick();
        break;
    default:
        break;
    }
}

bool OperationsManagementProtocol::s3Tick()// todo - use protocol state or s3 behavior state?
{
    switch (state) {
    case ProtocolStates::WAITING_REPLY:
    {

        VSMMessage* res = behaviour->receive(MessageContents::AGREE);
        if(res!= 0){
            //enter state waiting formation complete
            state = ProtocolStates::WAITING_FORMATION_COMPLETE;
            delete res;
        break;
        }
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
            delete res;
        }
    }
        break;
    }
    return false;
}

bool OperationsManagementProtocol::s2Tick()
{
    switch (state) {
   case ProtocolStates::WAITING_START_REQUEST:{
        VSMMessage* res = behaviour->receive(MessageContents::FIRST_FORMATION_START);
        if(res!= 0){
            //owner s2 should send beacon info queries to see if there is enough beacons, choose all three beacons and make first of them to start tpfp
       cout<<"S2 beacons received -start first formation \n";

       ((S2BeaconsBehaviour*)behaviour)->beaconManagementProtocol->start();
       //send confirmation to s3 - todo
       state = ProtocolStates::WAITING_FORMATION_COMPLETE;// same state as s3
            delete res;
        }}
        break;
   case ProtocolStates::WAITING_FORMATION_COMPLETE:
        break;
    }
    return false;
}
