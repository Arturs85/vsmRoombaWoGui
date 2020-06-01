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

void BaseProtocol::querryWithTimeout(VSMMessage message,ProtocolStates onReply, ProtocolStates noReply, int retries, int timeout)
{
    querrieRetryCounter=0;
    querrieTicksCounter=0;
    querryMessage = message;
    onPositiveQuerry=onReply;
    onNoReply = noReply;
    querieTicksMax = timeout;
    querryRetriesMax = retries;
    behaviour->owner->sendMsg(message);
    state =ProtocolStates::QUERRY_WAITING_REPLY;

}



bool BaseProtocol::tick(){//call this in subclass for querryWithTimeout feature to work
    switch (state) {
    case ProtocolStates::QUERRY_WAITING_REPLY:{
        querrieTicksCounter++;
        if(querrieTicksCounter>querieTicksMax){
            if(querrieRetryCounter++ > querryRetriesMax){

            }
        }

    }
    }
}

void BaseProtocol::start()
{

}
