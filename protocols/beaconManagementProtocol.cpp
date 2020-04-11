#include "beaconManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"



BeaconManagementProtocol::BeaconManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
if(roleInProtocol==RoleInProtocol::RESPONDER)// responder needs to listen for requests in this topic, initiator will receive direct messages
    behaviour->subscribeToTopic(Topics::CVAL_TO_S2);
else
    behaviour->subscribeToTopic(Topics::CVAL_INC_REQUEST);
}



bool BeaconManagementProtocol::tick()// todo modify from source copy
{
    //cout<<"cvp tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::SENDER:
        return   senderTick();
        break;
    case RoleInProtocol::RECEIVER:
        return   receiverTick();
        break;

    default:
        break;
    }
    return false;
}


