#include "roleCheckingProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"

#define REPLY_WAITING_TICKS 5/TICK_PERIOD_SEC
#define NUMBER_OF_RETRYS 3

RoleCheckingProtocol::RoleCheckingProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
if(roleInProtocol==RoleInProtocol::RESPONDER)// responder needs to listen for requests in this topic, initiator will receive direct messages
    behaviour->subscribeToTopic(Topics::S3_ROLE_CHECKING);
else
    behaviour->subscribeToDirectMsgs();
}

bool RoleCheckingProtocol::tick()
{
    //cout<<"rcp tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::INITIATOR:
        return   initiatorTick();
        break;
    case RoleInProtocol::RESPONDER:
        return   responderTick();
        break;

    default:
        break;
    }

}

void RoleCheckingProtocol::start()
{
    //state = STARTED;
    //send message to s3 asking for role,
    waitTicksCounter=0;

    VSMMessage request(behaviour->owner->id,Topics::S3_ROLE_CHECKING,MessageContents::ROLE_CHECK_WITH_S3,"roleCh");

    behaviour->owner->sendMsg(request);// null ptr check, change to send
    
    state = ProtocolStates::WAITING_REPLY;
}

bool RoleCheckingProtocol::initiatorTick()
{
    bool ended = false;
    switch (state) {
    case ProtocolStates::STARTED:

        break;
    case ProtocolStates::WAITING_REPLY:{
        // see if there is reply in behaviours queue
        VSMMessage* res = behaviour->receive(MessageContents::S3REPLY_TO_ROLE_CHECK);
        if(res!= 0){ //reply has been received, start behaviour acording to the message
            VSMSubsystems roleToTake = static_cast<VSMSubsystems>(stoi(res->content));
            if(roleToTake!=VSMSubsystems::NONE){
                // todo add role

            }
            wasSuccessful = true;
            ended= true;
        }
        waitTicksCounter++;
        if(waitTicksCounter>=REPLY_WAITING_TICKS){
            // reply waiting timeot
            if(retrysSoFar<NUMBER_OF_RETRYS){// repeat query
                cout<<"-----no reply from s3 after "<<retrysSoFar<<" retries \n";
                retrysSoFar++;
                start();
            }else{// no response from s3, take this role
                state=ProtocolStates::FINISHED;
                behaviour->owner->addBehaviour(new S3Behaviour(behaviour->owner));
                wasSuccessful = false;
                ended= true;
            }
        }
    }
        break;


    default:
        break;
    }
    return ended;
}

bool RoleCheckingProtocol::responderTick()
{// just see if there is query and reply to it
    VSMMessage* res= behaviour->receive(MessageContents::ROLE_CHECK_WITH_S3);
    if(res!=0){
        //see if there is some role to delegate to requesting agent, send reply adressed directly to requester
        cout<<"rcp responder received query from "<<res->senderNumber<<std::endl;
        VSMMessage request(VSMSubsystems::S3,res->senderNumber,MessageContents::S3REPLY_TO_ROLE_CHECK,to_string((int)VSMSubsystems::NONE));
        behaviour->owner->sendMsg(request);// null ptr check

    }

    return false;
}
