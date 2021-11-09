#include "beaconManagementProtocolStatic.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include "baseS1ManagementProtocol.hpp"
#include "moveToTargetBehaviour.hpp"
#include "s1ExchangeProtocol.hpp"

BeaconManagementProtocolStatic::BeaconManagementProtocolStatic(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseS1ManagementProtocol(roleInProtocol,ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S2BEACON)// responder needs to listen for requests in this topic, initiator will receive direct messages
    {  behaviour->subscribeToTopic(Topics::TO_S2BEACONS);
    }
    else if (roleInProtocol==RoleInProtocol::BEACON){
        behaviour->subscribeToTopic(Topics::S2_TO_BEACONS);
        behaviour->subscribeToDirectMsgs();
    }
    else if (roleInProtocol==RoleInProtocol::MOVING_BEACON){
        behaviour->subscribeToDirectMsgs();
    }
    state = ProtocolStates::IDLE;
}

void BeaconManagementProtocolStatic::start(){
    if(roleInProtocol==RoleInProtocol::S2BEACON)
        querryBeacons();
}

bool BeaconManagementProtocolStatic::tick()// todo modify from source copy
{
    //BaseS1ManagementProtocol::tick();
    // cout<<"bmp tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::S2BEACON:
        return   managerTick();

        break;
    case RoleInProtocol::BEACON:
        return   beaconTick();
        // cout<<"bmp beacon tick returned \n";

        break;

    case RoleInProtocol::MOVING_BEACON:
        return   targetGoerBeaconTick();
        break;

    default:
        break;
    }


    return false;
}

int BeaconManagementProtocolStatic::getUnusedBeaconId()
{
    //check if from all available s1 someone is not used as beacon, i.e. is not b1,b2,and bm
    std::set<int>::iterator it = availableRobotsSet.begin();
    // Iterate till the end of set
    while (it != availableRobotsSet.end())
    {
        if(usedRobots.find(*it)==usedRobots.end())
        {
            return *it;
        }
        it++;
    }
    return 0;
}

void BeaconManagementProtocolStatic::startReformation(vector<int> cords)//for s2
{//wait while we have 3 more beacons available
    state = ProtocolStates::GATHERING_BEACONS;
    this->cords = cords;
}

//void BeaconManagementProtocol::sendChangeType(int robotId, VSMSubsystems s1NewType)// to call from outside of class
//{
//    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::BEACON_ROLE,std::to_string((int)s1NewType));
//    behaviour->owner->sendMsg(roleRequest);

//}

bool BeaconManagementProtocolStatic::managerTick()//just get 3 beacons
{// receive messages independing of state
    // cout<<"bmp manager tick ,state  "<< (int)state<<"\n";
    BaseProtocol::tick();
    VSMMessage* res= behaviour->receive(MessageContents::BEACONS_RQ);// use none content description, because there should be only one type of msg in this topic
    if(res!=0){
        // add senders id to beacons list
        availableRobotsSet.insert(res->senderNumber);
        cout<<"bmp manager- availablebeaconssize: "<<availableRobotsSet.size()<<"\n";
        ((S2BeaconsBehaviour*)behaviour)->lastS1Count=availableRobotsSet.size();//set this value in behaviour for other protocols to use it
    }

    switch (state) {
    case ProtocolStates::WAITING_REPLY:{

        if(availableRobotsSet.size()>=3){
            //send roles to beacons
            std::vector<int> avb(availableRobotsSet.begin(), availableRobotsSet.end()); //convert set to vector to acces elements

            usedRobots.insert(avb.at(0));
            usedRobots.insert(avb.at(1));
            usedRobots.insert(avb.at(2));

            // state = ProtocolStates::WAITING_CONFIRM_ROLE;
            state = ProtocolStates::WAITING_FORMATION_COMPLETE;//bypassing beacons role confirmation

        }
    }

        break;



    case ProtocolStates::IDLE://todo
        break;


    case ProtocolStates::GATHERING_BEACONS:{

        if(askingIntervalCounter++%5==0)//todo number to header
            ((S2BaseBehavior*)behaviour)->s1ExchangeProtocol->askS1();

        std::set<int> unusedRobots = getUnusedRobotsSet();
        if(unusedRobots.size()>=1){
            std::cout<<"bmp s2 sending to new beacon "<<currentRobotId<< "to add move roles \n";
            currentRobotId = *(unusedRobots.begin());
            VSMMessage roleRequest(behaviour->owner->id,currentRobotId,MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::TARGET_MOVING_BEACON));// reply to querry, could send some additional info, e.g. bat level
            querryWithTimeout(roleRequest,MessageContents::AGREE_MOVE_TO_TARGET,ProtocolStates::SENDING_CORDINATES,ProtocolStates::GATHERING_BEACONS,5,3);

            usedRobots.insert(currentRobotId);

            regroupingBeaconsSet.insert(currentRobotId);// to distinct between old and new beacons

        }
    } break;


    }
    return false;
}

bool BeaconManagementProtocolStatic::beaconTick()
{
    //  cout<<"bmp beacon tick ,state  "<< (int)state<<"\n";
    BaseS1ManagementProtocol::tick();
    switch (state) {
    case ProtocolStates::IDLE:{
        VSMMessage* res= behaviour->receive(MessageContents::QUERRY_INFO);// use none content description, because there should be only one type of msg in this topic
        if(res!=0){// reply to querry
            VSMMessage replyToQuerry(behaviour->owner->id,Topics::TO_S2BEACONS,MessageContents::BEACONS_RQ,"rq");// reply to querry, could send some additional info, e.g. bat level
            behaviour->owner->sendMsg(replyToQuerry);
            // state = ProtocolStates::WAITING_REPLY;
            std::cout<<"bmp beacon replying to s2 querry\n";

        }// try to receive order


    }
        break;
    }
    return false;
}



void BeaconManagementProtocolStatic::querryBeacons()
{
    std::cout<<"querry beacons called\n";

    VSMMessage queryBeaconS1(VSMSubsystems::S2_BEACONS,Topics::S2_TO_BEACONS,MessageContents::QUERRY_INFO,"q");
    behaviour->owner->sendMsg(queryBeaconS1);
    state = ProtocolStates::WAITING_REPLY;
}


