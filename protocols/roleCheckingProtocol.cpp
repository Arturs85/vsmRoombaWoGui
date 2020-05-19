#include "roleCheckingProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"

#define REPLY_WAITING_TICKS 5/TICK_PERIOD_SEC
#define NUMBER_OF_RETRYS 3
#define REQUEST_INTERVAL_TICKS 10/TICK_PERIOD_SEC //ten seconds


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
    intervalCounter=REQUEST_INTERVAL_TICKS;//descending counter
    //retrysSoFar=0;
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
            cout<<"rcp init received take role: "<<(int)roleToTake<<" \n";
            if(roleToTake!=VSMSubsystems::NONE){
                // todo add role
                behaviour->owner->addBehaviour(roleToTake);
            }
            state= ProtocolStates::FINISHED;
            retrysSoFar =0;
            wasSuccessful = true;
            ended= true;
            delete res;
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
                behaviour->owner->addBehaviour(VSMSubsystems::S3);// role checking behavior now needs to bee removed (by addBehaviour())
                wasSuccessful = false;
                ended= true;
            }
        }
    }
        break;
    case ProtocolStates::FINISHED:{
        // count till next ping.i.e.  role check
        intervalCounter--;

        if(intervalCounter<=0)
            start();
    }
    default:
        break;
    }
    return ended;// not ment to execute
}

bool RoleCheckingProtocol::responderTick()
{// just see if there is query and reply to it
    VSMMessage* res= behaviour->receive(MessageContents::ROLE_CHECK_WITH_S3);
    if(res!=0){
        //see if there is some role to delegate to requesting agent, send reply adressed directly to requester
        cout<<"rcp responder received query from "<<res->senderNumber<<std::endl;
        if(res->senderNumber!=behaviour->owner->id){//dont give roles to itsef - s3
            //get unfilled role
            VSMSubsystems unfilled = ((S3Behaviour*)behaviour)->getUnfilledRole();
            if(unfilled!=VSMSubsystems::NONE){
                //mark it as filled
                ((S3Behaviour*)behaviour)->markAsFilled(unfilled,res->senderNumber);//  todo mar as filled after received confirm

            }
            // send reply to initiator agent
            VSMMessage request(VSMSubsystems::S3,res->senderNumber,MessageContents::S3REPLY_TO_ROLE_CHECK,to_string((int)unfilled));
            behaviour->owner->sendMsg(request);// null ptr check

            //mark that sender is present- update its time, or add new entry if it is not present on the list
            std::map< int,double  >* presentRobots =  &((S3Behaviour*)behaviour)->knownAgents;

            auto search = presentRobots->find(res->senderNumber);

            if (search != presentRobots->end()) {
                presentRobots->at(res->senderNumber)=behaviour->owner->getSystemTimeSec();
            } else {
                presentRobots->emplace(res->senderNumber,behaviour->owner->getSystemTimeSec());

            }





        }
        delete res;
    }

    return false;
}
