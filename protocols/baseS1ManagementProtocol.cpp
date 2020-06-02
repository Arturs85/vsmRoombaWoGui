#include "baseS1ManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include <algorithm>
#include <iterator>

BaseS1ManagementProtocol::BaseS1ManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
//behaviour->subscribeToDirectMsgs();
}

void BaseS1ManagementProtocol::start()
{

}



bool BaseS1ManagementProtocol::tick()
{
    VSMMessage* res2= behaviour->receive(MessageContents::S1_CHANGE_TYPE);// use none content description, because there should be only one type of msg in this topic
    if(res2 !=0){
        VSMSubsystems role = static_cast<VSMSubsystems>(std::stoi(res2->content));
        std::cout<<"s1mp robot received role"<<res2->content <<"\n";
        // todo inform agent to start (add) coresp. behaviour and start protocol
        behaviour->owner->setS1Type(role);// this call also adds type to agent, so it can advertisize via rcp correctly
    }
    return false;
}

int BaseS1ManagementProtocol::getUnusedBeaconId()
{
    //todo

}

void BaseS1ManagementProtocol::sendChangeType(int robotId,std::string vSMSubsystemsS1NewTypeString)// to call from outside of class
{
    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::S1_CHANGE_TYPE,vSMSubsystemsS1NewTypeString);
    behaviour->owner->sendMsg(roleRequest);
    availableRobotsSet.erase(robotId);// mark that robot is no longer available
}

std::set<int> BaseS1ManagementProtocol::getUnusedRobotsSet()
{
    std::set<int> result;
    std::set_difference(availableRobotsSet.begin(), availableRobotsSet.end(), usedRobots.begin(), usedRobots.end(),
                        std::inserter(result, result.end()));

    return result;
}
