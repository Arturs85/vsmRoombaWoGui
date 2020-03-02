#include "roombaAgent.hpp"
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include "s3Behaviour.hpp"

string RoombaAgent::getRoleListForMsg()
{
    std::stringstream ss;
    ss<<(int)id;
    for (VSMSubsystems role: roleList) {
        ss<<","<<(int)role;
    }
    cout<<"roleList: "<<ss.str()<<"\n";
    return ss.str();

}

void RoombaAgent::initHardware(){
    uwbMsgListener.initialize(id);
    uwbMsgListener.startReceiving();
    uwbMsgListener.startSending();

    uartTest.initialize();
    uartTest.startReceiveing();//starts receiving and sending threads
    roombaController = new RoombaController(&uartTest);
    roombaController->startFull();
}

RoombaAgent::RoombaAgent()
{
    getId();
    roleList.push_back(VSMSubsystems::S1);//for test
    roleList.push_back(VSMSubsystems::S3);//for test
getRoleListForMsg();
vector<BaseCommunicationBehaviour*> init;
subscribersMap.resize(Topics::SIZE_OF_THIS_ENUM,init);// initialize map with empty lists
}

void RoombaAgent::getId()
{
    char hostname[HOST_NAME_MAX];
    int res =gethostname(hostname, HOST_NAME_MAX);
    if(!res){
        printf ("device hostname: %s\n",hostname);
        std::string subStr = string(hostname).substr (4,5);     // id position in hostname
        int r=0;
        try{
            r = stoi(subStr);
        }
        catch(std::invalid_argument& e){
            std::cout<< "cant convert to int\n";
        }
        id = (uint8_t)r;
    }
    else
        printf ("unable to get device hostname\n");

    cout<<" Id from hostname: "<<id+0<<"\n";

}

void RoombaAgent::advertise()// send msg with own id and role list
{
    uwbMsgListener.addToTxDeque(getRoleListForMsg());
}

void RoombaAgent::distributeMessages()// copy received messages from uwblistener to subscribers queues
{
   VSMMessage* msg = uwbMsgListener.getFirstRxMessageFromDeque();
   Topics rec = msg->receiver;// change to topics
   vector<BaseCommunicationBehaviour*> subs = subscribersMap.at(rec);

   if(subs.empty()) delete msg;// delete msg if there is no subscribers for it

   for (int i =0; i<subs.size();i++) {
subs.at(i)->msgDeque.push_back(msg);
   }
}

void RoombaAgent::startCycle()
{
    while(isRunning){
        // run behaviours sequentially
        behavioursStep();

    }
}


void RoombaAgent::behavioursStep()
{
    for (BaseCommunicationBehaviour* b : behavioursList) {
        b->behaviourStep();
    }
}

void RoombaAgent::addBehaviour(BaseCommunicationBehaviour *bcb)//add new behaviour and remove conflicting existing ones
{
vector<std::string> conf = conflictingBehaviours.at(typeid(bcb).name());
for (string s: conf ) {
    BaseCommunicationBehaviour* b =findBehaviourByName(s);
    // call behaviours remove
b->remove();
//delete b;
}
behavioursList.push_back(bcb);
string behName =typeid(bcb).name();
if(behName.compare("S3Behaviour")) isS3 = true; //todo - hardcoded name
}

void RoombaAgent::removeBehaviour(string name)
{

    vector< BaseCommunicationBehaviour* >::iterator it = behavioursList.begin();

    while(it != behavioursList.end()) {
        string bName = (typeid(it).name());

          if(bName.compare(name)==0)
         {

            it = behavioursList.erase(it);
        }
        else ++it;
    }


}

void RoombaAgent::removeBehaviour(BaseCommunicationBehaviour *bcb)
{
    behavioursList.erase(std::remove(behavioursList.begin(), behavioursList.end(), bcb), behavioursList.end());// remove by value
}

void RoombaAgent::sendMsg(VSMMessage msg)
{
    // check wether receiver topic is present on this agent, if so put msg directly in its queue

}

BaseCommunicationBehaviour *RoombaAgent::findBehaviourByName(string name)
{
    for (int i = 0; i < behavioursList.size(); ++i) {
        string bName = (typeid(behavioursList.at(i)).name());

          if(bName.compare(name)==0){
              return behavioursList.at(i);
          }
    }
    return 0;
}
