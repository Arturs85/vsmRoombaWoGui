#include "beaconManagementProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"


BeaconManagementProtocol::BeaconManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
    if(roleInProtocol==RoleInProtocol::S2BEACON)// responder needs to listen for requests in this topic, initiator will receive direct messages
        behaviour->subscribeToTopic(Topics::CVAL_TO_S2);
    else
        behaviour->subscribeToTopic(Topics::S2_TO_BEACONS);
}

void BeaconManagementProtocol::start(){
	
	}

bool BeaconManagementProtocol::tick()// todo modify from source copy
{
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

bool BeaconManagementProtocol::managerTick()
{
    switch (state) {
    case ProtocolStates::WAITING_REPLY:{
        VSMMessage* res= behaviour->receive(MessageContents::NONE);// use none content description, because there should be only one type of msg in this topic
        if(res!=0){
            // add senders id to beacons list
            availableBeaconsSet.insert(res->senderNumber);
            if(availableBeaconsSet.size()>=2){
                //send roles to beacons
               std::vector<int> avb(availableBeaconsSet.begin(), availableBeaconsSet.end()); //convert set to vector to acces elements
                VSMMessage roleRequest(behaviour->owner->id,avb.at(0),MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::BEACON_ONE));// reply to querry, could send some additional info, e.g. bat level
                VSMMessage roleRequest2(behaviour->owner->id,avb.at(1),MessageContents::BEACON_ROLE,std::to_string((int)VSMSubsystems::BEACON_TWO));// reply to querry, could send some additional info, e.g. bat level

                behaviour->owner->sendMsg(roleRequest);
                behaviour->owner->sendMsg(roleRequest2);
                state = ProtocolStates::WAITING_CONFIRM_ROLE;
            }
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

bool BeaconManagementProtocol::beaconTick()
{
    switch (state) {
    case ProtocolStates::IDLE:{
        VSMMessage* res= behaviour->receive(MessageContents::NONE);// use none content description, because there should be only one type of msg in this topic
        if(res!=0){// reply to querry
            VSMMessage replyToQuerry(behaviour->owner->id,Topics::TO_S2BEACONS,MessageContents::NONE,0);// reply to querry, could send some additional info, e.g. bat level
            behaviour->owner->sendMsg(replyToQuerry);
            // state = ProtocolStates::WAITING_REPLY;


        }// try to receive order
        VSMMessage* res2= behaviour->receive(MessageContents::BEACON_ROLE);// use none content description, because there should be only one type of msg in this topic
        if(res!=0){
            VSMSubsystems role = static_cast<VSMSubsystems>(std::stoi(res2->content));
            std::cout<<"bmp beacon received role";
            // todo inform agent to start (add) coresp. behaviour and start protocol
       behaviour->owner->addBehaviour(role);
        }

    }
        break;
    }
}

void BeaconManagementProtocol::querryBeacons()
{
    VSMMessage queryBeaconS1(VSMSubsystems::S2_BEACONS,Topics::S2_TO_BEACONS,MessageContents::QUERRY_INFO,0);
    behaviour->owner->sendMsg(queryBeaconS1);
    state = ProtocolStates::WAITING_REPLY;
}


