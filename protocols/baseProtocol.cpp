#include "baseProtocol.hpp"
#include "roombaAgent.hpp"

BaseProtocol::BaseProtocol(BaseCommunicationBehaviour *ownerBeh)
{
    this->behaviour = ownerBeh;
}

vector<int> BaseProtocol::stringTointVector(string s){
    vector<int> res;
    std::stringstream ss(s);
    int i;

    while(ss>>i){

        //if(ss>>i)
        res.push_back(i);

    }
    return  res;
}

string BaseProtocol::intVectorToString(vector<int> iv)
{
    std::stringstream ss;
    for(auto i:iv){
        ss<< i<<" ";
    }
    return ss.str();
}

void BaseProtocol::querryWithTimeout(VSMMessage message,MessageContents replyContents,ProtocolStates onReply, ProtocolStates noReply, int retries, int timeout)
{
    receivedReplyOnQuerry=0;//delete this message erlier, now only mark it as 0
    querrieRetryCounter=0;
    querrieTicksCounter=0;
    querryMessage = message;
    expectedReplyContents= replyContents;
    onPositiveQuerry=onReply;
    onNoReply = noReply;
    querieTicksMax = timeout;
    querryRetriesMax = retries;
    behaviour->owner->sendMsg(message);
    state = ProtocolStates::QUERRY_WAITING_REPLY;

}



bool BaseProtocol::tick(){//call this in subclass for querryWithTimeout feature to work
    switch (state) {
    case ProtocolStates::QUERRY_WAITING_REPLY:{
      std::cout<<"BP tick state qwr \n";
        receivedReplyOnQuerry = behaviour->receive(expectedReplyContents);
        if(receivedReplyOnQuerry!=0){
            state = onPositiveQuerry;
        }
        querrieTicksCounter++;
        if(querrieTicksCounter>querieTicksMax){
            querrieTicksCounter=0;
            if(querrieRetryCounter++ > querryRetriesMax){
                state =onNoReply;
            }else{//resend querry
                std::cout<<"retry querry "<<querrieRetryCounter<<"\n";
                behaviour->owner->sendMsg(querryMessage);

            }
        }

    }break;
    }
    return false;
}

void BaseProtocol::start()
{

}
