#include "controlValueProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "s2BaseBehaviour.hpp"


ControlValueProtocol::ControlValueProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;
if(roleInProtocol==RoleInProtocol::RESPONDER)// responder needs to listen for requests in this topic, initiator will receive direct messages
    behaviour->subscribeToTopic(Topics::CVAL_TO_S2);
else
    behaviour->subscribeToTopic(Topics::CVAL_INC_REQUEST);
}

void ControlValueProtocol::start()
{

}

bool ControlValueProtocol::tick()
{
    //cout<<"cvp tick \n";
    switch (roleInProtocol) {
    case RoleInProtocol::SENDER:
        return   senderTick();
        break;
    case RoleInProtocol::RECEIVER:
        return   receiverTick();
        break;

    default:
        break;
    }
    return false;
}

void ControlValueProtocol::sendCvals(vector<int> cVals)
{
    //convert vector to string for sending as chars via radio
    std::string cValsString = BaseProtocol::intVectorToString(cVals);
    //send message
    // send reply to initiator agent
    VSMMessage cValsMsg(VSMSubsystems::S3,Topics::CVAL_TO_S2,MessageContents::NONE,cValsString);
    behaviour->owner->sendMsg(cValsMsg);// null ptr check?
}

bool ControlValueProtocol::senderTick()// for s3, aka initiator or sender ---todo implement
{
tickCount++;
if(tickCount%10==0){// send cVals to s2 every 10 seconds, if tick is one second
sendCvals(((S3Behaviour*)behaviour)->cvals);
}

    //listen for increase requests

    return false;
}

bool ControlValueProtocol::receiverTick()//for s2, aka responder or receiver
{
// check for new cValues
    VSMMessage* res= behaviour->receive(MessageContents::NONE);// use none content description, because there should be only one type of msg in this topic
    if(res!=0){
        cVals = BaseProtocol::stringTointVector(res->content);
   ((S2BaseBehavior*)behaviour)->lastControlValue=cVals.at(((S2BaseBehavior*)behaviour)->s2type);
    }
    return 0;
}
