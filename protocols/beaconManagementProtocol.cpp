#include "beaconManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include "baseS1ManagementProtocol.hpp"

BeaconManagementProtocol::BeaconManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseS1ManagementProtocol(roleInProtocol,ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S2BEACON)// responder needs to listen for requests in this topic, initiator will receive direct messages
    {  behaviour->subscribeToTopic(Topics::TO_S2BEACONS);
    }
    else{
        behaviour->subscribeToTopic(Topics::S2_TO_BEACONS);
        behaviour->subscribeToDirectMsgs();
        state = ProtocolStates::IDLE;
    }
}

void BeaconManagementProtocol::start(){
    if(roleInProtocol==RoleInProtocol::S2BEACON)
        querryBeacons();
}

bool BeaconManagementProtocol::tick()// todo modify from source copy
{
   //BaseS1ManagementProtocol::tick();
    //cout<<"cvp tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::S2BEACON:
        return   managerTick();
        break;
    case RoleInProtocol::BEACON:
        return   beaconTick();
        break;

    default:
        break;
    }
    return false;
}

int BeaconManagementProtocol::getUnusedBeaconId()
{
//check if from all available s1 someone is not used as beacon, i.e. is not b1,b2,and bm
    std::set<int>::iterator it = availableBeaconsSet.begin();
    // Iterate till the end of set
    while (it != availableBeaconsSet.end())
    {
      if(usedBeacons.find(*it)!=usedBeacons.end())
      {
       return *it;
      }
      it++;
    }
return 0;
}

//void BeaconManagementProtocol::sendChangeType(int robotId, VSMSubsystems s1NewType)// to call from outside of class
//{
//    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::BEACON_ROLE,std::to_string((int)s1NewType));
//    behaviour->owner->sendMsg(roleRequest);

//}

bool BeaconManagementProtocol::managerTick()//todo add reply waiting timeout and send requests again
{// receive messages independing of state
    VSMMessage* res= behaviour->receive(MessageContents::NONE);// use none content description, because there should be only one type of msg in this topic
    if(res!=0){
        // add senders id to beacons list
        availableBeaconsSet.insert(res->senderNumber);
        cout<<"bmp manager- availablebeaconssize: "<<availableBeaconsSet.size()<<"\n";
        ((S2BeaconsBehaviour*)behaviour)->lastS1Count=availableBeaconsSet.size();//set this value in behaviour for other protocols to use it
    }

    switch (state) {
    case ProtocolStates::WAITING_REPLY:{

        if(availableBeaconsSet.size()>=3){
            //send roles to beacons
            std::vector<int> avb(availableBeaconsSet.begin(), availableBeaconsSet.end()); //convert set to vector to acces elements
            VSMMessage roleRequest(behaviour->owner->id,avb.at(0),MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::BEACON_ONE));// reply to querry, could send some additional info, e.g. bat level
            VSMMessage roleRequest2(behaviour->owner->id,avb.at(1),MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::BEACON_TWO));// reply to querry, could send some additional info, e.g. bat level
            VSMMessage roleRequest3(behaviour->owner->id,avb.at(2),MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::BEACON_MASTER));// reply to querry, could send some additional info, e.g. bat level

            behaviour->owner->sendMsg(roleRequest);
            behaviour->owner->sendMsg(roleRequest2);
            behaviour->owner->sendMsg(roleRequest3);
usedBeacons.insert(avb.at(0));
usedBeacons.insert(avb.at(1));
usedBeacons.insert(avb.at(2));

            state = ProtocolStates::WAITING_CONFIRM_ROLE;

        }
    }

        break;
    case ProtocolStates::WAITING_CONFIRM_ROLE:{
        VSMMessage* res= behaviour->receive(MessageContents::CONFIRM_ROLE);
        if(res!=0){
            VSMSubsystems role = static_cast<VSMSubsystems>(std::stoi(res->content));
            switch (role) {
            case VSMSubsystems::BEACON_ONE:
                bOneIsFilled = true;
                break;
            case VSMSubsystems::BEACON_TWO:
                bTwoIsFilled = true;
                break;
            case VSMSubsystems::BEACON_MASTER:
                bMasterIsFilled = true;
                break;
            }
            if(bOneIsFilled && bTwoIsFilled && bMasterIsFilled) state = ProtocolStates::WAITING_FORMATION_COMPLETE;
            std::cout<<"bmp all beacon roles filled \n";
        }
    }
    case ProtocolStates::WAITING_FORMATION_COMPLETE:{
        VSMMessage* res= behaviour->receive(MessageContents::FORMATION_COMPLETED);
        if(res!=0){
        std::cout<<"bmp master(s2) received formation complete\n";
        VSMMessage formDone(behaviour->owner->id,Topics::S3_IN,MessageContents::FORMATION_COMPLETED,"fc");//resend this info to s3
        behaviour->owner->sendMsg(formDone);

        //wait while explorers are operating
        state= ProtocolStates::IDLE;
    }
}
        break;

    case ProtocolStates::IDLE://todo
break;
    }
}

bool BeaconManagementProtocol::beaconTick()
{
    switch (state) {
    case ProtocolStates::IDLE:{
        VSMMessage* res= behaviour->receive(MessageContents::QUERRY_INFO);// use none content description, because there should be only one type of msg in this topic
        if(res!=0){// reply to querry
            VSMMessage replyToQuerry(behaviour->owner->id,Topics::TO_S2BEACONS,MessageContents::NONE,"rq");// reply to querry, could send some additional info, e.g. bat level
            behaviour->owner->sendMsg(replyToQuerry);
            // state = ProtocolStates::WAITING_REPLY;
            std::cout<<"bmp beacon replying to s2 querry\n";

        }// try to receive order
        VSMMessage* res2= behaviour->receive(MessageContents::BEACON_ROLE);// use none content description, because there should be only one type of msg in this topic
        if(res2 !=0){
            VSMSubsystems role = static_cast<VSMSubsystems>(std::stoi(res2->content));
            std::cout<<"bmp beacon received role"<<res2->content <<"\n";
            // todo inform agent to start (add) coresp. behaviour and start protocol
            behaviour->owner->addBehaviour(role);
        }
     
    }
        break;
    }
}

void BeaconManagementProtocol::querryBeacons()
{
    VSMMessage queryBeaconS1(VSMSubsystems::S2_BEACONS,Topics::S2_TO_BEACONS,MessageContents::QUERRY_INFO,"q");
    behaviour->owner->sendMsg(queryBeaconS1);
    state = ProtocolStates::WAITING_REPLY;
}


