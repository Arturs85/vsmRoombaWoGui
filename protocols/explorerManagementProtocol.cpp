#include "explorerManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include "baseS1ManagementProtocol.hpp"

ExplorerManagementProtocol::ExplorerManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseS1ManagementProtocol(roleInProtocol,ownerBeh)
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

void ExplorerManagementProtocol::start(){
    if(roleInProtocol==RoleInProtocol::S2BEACON){}
        //querryBeacons();
}

bool ExplorerManagementProtocol::tick()// todo modify from source copy
{

    return false;
}

int ExplorerManagementProtocol::getUnusedBeaconId()
{
    //todo
}

//void BeaconManagementProtocol::sendChangeType(int robotId, VSMSubsystems s1NewType)// to call from outside of class
//{
//    VSMMessage roleRequest(behaviour->owner->id,robotId,MessageContents::BEACON_ROLE,std::to_string((int)s1NewType));
//    behaviour->owner->sendMsg(roleRequest);

//}

bool ExplorerManagementProtocol::managerTick()//todo add reply waiting timeout and send requests again
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
    case ProtocolStates::WAITING_FORMATION_COMPLETE:
        break;

    }
}

bool ExplorerManagementProtocol::explorerTick()
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

void ExplorerManagementProtocol::querryExplorers()
{
    VSMMessage queryBeaconS1(VSMSubsystems::S2_BEACONS,Topics::S2_TO_BEACONS,MessageContents::QUERRY_INFO,"q");
    behaviour->owner->sendMsg(queryBeaconS1);
    state = ProtocolStates::WAITING_REPLY;
}


