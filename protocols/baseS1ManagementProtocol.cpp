#include "baseS1ManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"

BaseS1ManagementProtocol::BaseS1ManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;

}



bool BaseS1ManagementProtocol::tick()// todo modify from source copy
{

    return false;
}

int BaseS1ManagementProtocol::getUnusedBeaconId()
{
    //todo
}

void BaseS1ManagementProtocol::sendChangeType(int robotId,std::string vSMSubsystemsS1NewTypeString)// to call from outside of class
{
    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::BEACON_ROLE,vSMSubsystemsS1NewTypeString);
    behaviour->owner->sendMsg(roleRequest);

}

