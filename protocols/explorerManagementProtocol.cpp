#include "explorerManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2ExplorersBehaviour.hpp"
#include "baseS1ManagementProtocol.hpp"
#include "s1ExchangeProtocol.hpp"
#include "explorerListenerBehaviour.hpp"

ExplorerManagementProtocol::ExplorerManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseS1ManagementProtocol(roleInProtocol,ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S2EXPLORERS)// responder needs to listen for requests in this topic, initiator will receive direct messages
    {  behaviour->subscribeToTopic(Topics::S2EXPLORERS_IN);
        state = ProtocolStates::IDLE;
    }
    else{
        behaviour->subscribeToTopic(Topics::S2_TO_EXPLORERS);
        behaviour->subscribeToDirectMsgs();
        state = ProtocolStates::IDLE;
    }
}

void ExplorerManagementProtocol::start(){
    if(roleInProtocol==RoleInProtocol::S2EXPLORERS){
        state = ProtocolStates::BEACONS_DEPLOYED;

    }
    //querryBeacons();
}

bool ExplorerManagementProtocol::tick()// todo modify from source copy
{
    switch (roleInProtocol) {
    case RoleInProtocol::S2EXPLORERS:
        return managerTick();
        break;
    case RoleInProtocol::EXPLORER:
        return explorerTick();
        break;

    default:
        break;
    }

    return false;
}

int ExplorerManagementProtocol::getUnusedBeaconId()
{
    //todo
}

void ExplorerManagementProtocol::enterIdleState()
{
    state= ProtocolStates::IDLE;
    sendStopExploring();
}

void ExplorerManagementProtocol::sendStopExploring()// without confirmation
{
    std::set<int>::iterator it = usedRobots.begin();
    // Iterate till the end of set
    while (it != usedRobots.end())
    {
        VSMMessage stopRequest(behaviour->owner->id,*it,MessageContents::STOP_EXPLORING,"st");// reply to querry, could send some additional info, e.g. bat level
        behaviour->owner->sendMsg(stopRequest);
        it++;
    }
}

//void BeaconManagementProtocol::sendChangeType(int robotId, VSMSubsystems s1NewType)// to call from outside of class
//{
//    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::BEACON_ROLE,std::to_string((int)s1NewType));
//    behaviour->owner->sendMsg(roleRequest);

//}

bool ExplorerManagementProtocol::managerTick()//todo add reply waiting timeout and send requests again
{// receive messages independing of state
    VSMMessage* res= behaviour->receive(MessageContents::BEACONS_RQ);
    if(res!=0){
        // add senders id to beacons list
        availableRobotsSet.insert(res->senderNumber);
        cout<<"emp manager- availableRobotsSize: "<<availableRobotsSet.size()<<"\n";
        ((S2ExplorersBehaviour*)behaviour)->lastS1Count=availableRobotsSet.size();//set this value in behaviour for other protocols to use it
    }


    switch (state) {
    case ProtocolStates::BEACONS_DEPLOYED:{

        // periodically ask for s1
        ((S2BaseBehavior*)behaviour)->s1ExchangeProtocol->askS1();
        //std::cout<<"ask s1 ok \n";

        // send start request to all new unused explorers (one by one each tick for now)
        std::set<int> unusedRobots = getUnusedRobotsSet();
        //std::cout<<"got unused robots set \n";
        if(unusedRobots.size()>=1){

            VSMMessage startRequest(behaviour->owner->id,*unusedRobots.begin(),MessageContents::START_EXPLORING,"seb");// reply to querry, could send some additional info, e.g. bat level
            behaviour->owner->sendMsg(startRequest);
            usedRobots.insert(*unusedRobots.begin());//for now mark it as employed without confirmation from robot itself
            std::cout<<"sent start exploring to robot "<<*unusedRobots.begin()<<"\n";

        }

    }
        break;

    case ProtocolStates::WAITING_FORMATION_COMPLETE:
        break;
    case ProtocolStates::IDLE:
        break;
    }
}

bool ExplorerManagementProtocol::explorerTick()
{
   BaseS1ManagementProtocol::tick();//listens to change type msgs
    //listen for stop/start commands
    BaseCommunicationBehaviour::logKeypoints("emp exp tick started\n");
    VSMMessage* res= behaviour->receive(MessageContents::START_EXPLORING);
    if(res!=0){
        ((ExplorerListenerBehaviour*)behaviour)->startExploring();
    }
    VSMMessage* res2= behaviour->receive(MessageContents::STOP_EXPLORING);
    if(res2!=0){
        ((ExplorerListenerBehaviour*)behaviour)->startExploring();
    }

    switch (state) {
    case ProtocolStates::IDLE:{


    }
        break;
    }
    return false;
}

void ExplorerManagementProtocol::querryExplorers()
{
    VSMMessage queryBeaconS1(VSMSubsystems::S2_BEACONS,Topics::S2_TO_BEACONS,MessageContents::QUERRY_INFO,"q");
    behaviour->owner->sendMsg(queryBeaconS1);
    state = ProtocolStates::WAITING_REPLY;
}


