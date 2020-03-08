#include "roombaAgent.hpp"
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include "s3Behaviour.hpp"
#include "roleCheckingBehaviour.hpp"
#include "baseCommunicationBehaviour.hpp"
#include <sys/time.h>
#include <typeinfo>
#define USLEEP_TIME_US 10000

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
    //roleList.push_back(VSMSubsystems::S1);//for test
    //roleList.push_back(VSMSubsystems::S3);//for test
    getRoleListForMsg();
    vector<BaseCommunicationBehaviour*> init;
    subscribersMap.resize(static_cast<int>(Topics::SIZE_OF_THIS_ENUM),init);// initialize map with empty lists
cout<<" size of subscribers map: "<<subscribersMap.size()<<"\n";
   

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
    int receiver=0;
    if(msg->receiver==Topics::NONE)// see wether adressing is topic or direct
        receiver = msg->receiverNumber;
    else
        receiver = (int)msg->receiver;

    vector<BaseCommunicationBehaviour*> subs = subscribersMap.at(receiver);

    if(subs.empty()) delete msg;// delete msg if there is no subscribers for it

    for (int i =0; i<subs.size();i++) {
        subs.at(i)->msgDeque.push_back(new VSMMessage(msg));
    }
}

void RoombaAgent::startCycle()
{
	 addBehaviour(new RoleCheckingBehaviour(this));
    while(isRunning){

//check if it is time for next step
        double time = getSystemTimeSec();
        double diff =time-lastTime;
        if(diff>TICK_PERIOD_SEC){
            // run behaviours sequentially
            //cout<<"dt: "<<(diff)<<"\n";
            behavioursStep();
			lastTime = getSystemTimeSec();
            //cout<<"ra timeNow "<<time<<"exec time: "<<(lastTime-time)<<"\n";

        }else{
            //sleep to allow contextSwitch
        usleep(USLEEP_TIME_US);
        }

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
//    vector<std::string> conf = conflictingBehaviours.at(typeid(bcb).name());

  //  for (string s: conf ) {
   //     BaseCommunicationBehaviour* b =findBehaviourByName(s);
        // call behaviours remove
  //      b->remove();
    //    //delete b;
   // }
    behavioursList.push_back(bcb);
    string behName =typeid(*bcb).name();
    if(behName.compare("S3Behaviour")) isS3 = true; //todo - hardcoded name
cout<<"added behaviour: "<<behName<<"\n";

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
    //try to send this msg to own behaviours
    // check wether receiver topic is present on this agent, if so put msg directly in its queue
    int receiver=0;
    if(msg.receiver==Topics::NONE)// see wether adressing is topic or direct
        receiver = msg.receiverNumber;
    else
        receiver = (int)msg.receiver;
 
    vector<BaseCommunicationBehaviour*> subs = subscribersMap.at(receiver);
 
        for (int i =0; i<subs.size();i++) {
        subs.at(i)->msgDeque.push_back(msg);
    }


        // send message to others
        uwbMsgListener.addToTxDeque(msg);//change to  pass by reference?

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

 double RoombaAgent::getSystemTimeSec(void){
    struct timeval start_time;
    double milli_time;
    gettimeofday(&start_time, NULL);

    milli_time = ((start_time.tv_usec) / 1000000.0 + start_time.tv_sec);
    return milli_time;
}
