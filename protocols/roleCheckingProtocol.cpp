#include "roleCheckingProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing

#define REPLY_WAITING_TICKS 5000/TICK_PERIOD_MS
#define NUMBER_OF_RETRYS 3

RoleCheckingProtocol::RoleCheckingProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;

}

bool RoleCheckingProtocol::tick()
{
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
    VSMMessage request(behaviour->owner->id,Topics::S3_,MessageContents::ROLE_CHECK_WITH_S3,"roleCh");
    behaviour->owner->sendMsg(request);// null ptr check, change to send
    state = ProtocolStates::WAITING_REPLY;
}

bool RoleCheckingProtocol::initiatorTick()
{

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
            return true;
        }
        waitTicksCounter++;
        if(waitTicksCounter>=REPLY_WAITING_TICKS){
            // reply waiting timeot
            if(retrysSoFar<NUMBER_OF_RETRYS){// repeat query
                cout<<"-----no reply from s3 after "<<retrysSoFar<<" retries \n";
                retrysSoFar++;
                start();
            }else{
                state=ProtocolStates::FINISHED;
                wasSuccessful = false;
                return true;
            }

        }
    }
        break;


    default:
        break;
    }

}

bool RoleCheckingProtocol::responderTick()
{// just see if there is query and reply to it
   VSMMessage* res= behaviour->receive(MessageContents::ROLE_CHECK_WITH_S3);
   if(res!=0){
       //see if there is some role to delegate to requesting agent, send reply adressed directly to requester

       VSMMessage request(VSMSubsystems::S3,res->senderNumber,MessageContents::S3REPLY_TO_ROLE_CHECK,to_string((int)VSMSubsystems::NONE));
       behaviour->owner->sendMsg(request);// null ptr check

   }

   return false;
}
