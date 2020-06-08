#include "roombaAgent.hpp"
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include "s3Behaviour.hpp"
#include "roleCheckingBehaviour.hpp"
#include "baseCommunicationBehaviour.hpp"
#include <sys/time.h>
#include <typeinfo>
#include "roombaMovementManager.hpp"
#include "beaconOneBehaviour.hpp"
#include "beaconTwoBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include "beaconListenerBehaviour.hpp"
#include "beaconMasterBehaviour.hpp"
#include "explorerListenerBehaviour.hpp"
#include "s2ExplorersBehaviour.hpp"
#include "s4Behaviour.hpp"
#include "moveToTargetBehaviour.hpp"
#include <iomanip>

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
    
    uint16_t ca = roombaController->readBattCapacity();
    uint16_t ch = roombaController->readBattCharge();
    std::cout<<"batt ca: "<<ca<< ", ch: "<<ch<<" left: "<<(100*ch/++ca)<<" %\n";

    movementManager = new RoombaMovementManager(roombaController);
    setS1Type(s1Type);// sets that same parameter, but also adds coresp. behaviour
    //std::cout<<"ra initHardware complete\n";

}

RoombaAgent::RoombaAgent()
{
 //   conflictingBehaviours.emplace(VSMSubsystems::S3,std::vector<VSMSubsystems>({VSMSubsystems::ROLE_CHECKING}));// where to initialize this?
    conflictingBehaviours.emplace(VSMSubsystems::S1_BEACONS,std::vector<VSMSubsystems>({VSMSubsystems::S1_EXPLORERS}));
    conflictingBehaviours.emplace(VSMSubsystems::S1_EXPLORERS,std::vector<VSMSubsystems>({VSMSubsystems::S1_BEACONS}));
    conflictingBehaviours.emplace(VSMSubsystems::TARGET_MOVING_BEACON,std::vector<VSMSubsystems>({VSMSubsystems::S1_EXPLORERS,VSMSubsystems::TARGET_MOVING_BEACON}));

    getId();
    //roleList.push_back(VSMSubsystems::S1);//for test
    //roleList.push_back(VSMSubsystems::S3);//for test
    getRoleListForMsg();
    set<BaseCommunicationBehaviour*> init;
    subscribersMap.resize(static_cast<int>(Topics::SIZE_OF_THIS_ENUM),init);// initialize map with empty lists
    cout<<" size of subscribers map: "<<subscribersMap.size()<<"\n";
UwbMsgListener::owner=this;
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
    if(msg == 0) return;
    if(msg->receiver==Topics::NONE)// see wether adressing is topic or direct
        receiver = msg->receiverNumber;
    else
        receiver = (int)msg->receiver;

    std::set<BaseCommunicationBehaviour*> subs = subscribersMap.at(receiver);

 //   if(subs.empty()) delete msg;// delete msg if there is no subscribers for it
    std::set<BaseCommunicationBehaviour*>::iterator it;

    for (it = subs.begin(); it != subs.end(); ++it) {
        (*it)->msgDeque.push_back(new VSMMessage(msg));
    }
     delete msg;// delete msg after it is copied to rec behaviours

    distributeMessages();//recursion
}

void RoombaAgent::startCycle()
{
    addBehaviour(VSMSubsystems::ROLE_CHECKING);
    while(isRunning){

        //check if it is time for next step
        double time = getSystemTimeSec();
        double diff =time-lastTime;
        if(diff>TICK_PERIOD_SEC){
            // run behaviours sequentially
            //cout<<"dt: "<<(diff)<<"\n";
            distributeMessages();
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
        BaseCommunicationBehaviour::logKeypoints("-------roomba agent behaviour step--------\n");

    for (BaseCommunicationBehaviour* b : behavioursList) {
        b->behaviourStep();
	}
        //add new roles 
    if(rolesToAdd.size()>0)
    {
    for (VSMSubsystems b : rolesToAdd) {
        addBehaviourToList(b);
    }
    rolesToAdd.clear();
    }
}
void RoombaAgent::addBehaviour(VSMSubsystems behaviour)//add new behaviour and remove conflicting existing ones
{
	rolesToAdd.push_back(behaviour);
	}
void RoombaAgent::addBehaviourToList(VSMSubsystems behaviour)
{
    


    auto search = conflictingBehaviours.find(behaviour);

    if (search != conflictingBehaviours.end()) {
        vector<VSMSubsystems> conf = conflictingBehaviours.at(behaviour);

        for (VSMSubsystems beh: conf ) {
            BaseCommunicationBehaviour* b =findBehaviourByName(beh);
            //  call behaviours remove
            if(b!=0)
                b->remove();
            //delete b;
        }    }

    BaseCommunicationBehaviour* bcb;
    switch (behaviour) {
    case VSMSubsystems::S3 :{
        if(isS3)return;//workaround for not adding more than one s3 behaviour
        bcb = new S3Behaviour(this);
    }break;
    case VSMSubsystems::ROLE_CHECKING:{
        bcb = new RoleCheckingBehaviour(this);
    }break;
    case VSMSubsystems::BEACON_ONE:{
        bcb = new BeaconOneBehaviour(this);
        uwbMsgListener.idFromBeaconType=(uint8_t)Topics::BEACON_ONE_IN;

    }break;
    case VSMSubsystems::BEACON_TWO:{
        bcb = new BeaconTwoBehaviour(this);
        uwbMsgListener.idFromBeaconType=(uint8_t)Topics::BEACON_TWO_IN;

    }break;
    case VSMSubsystems::S2_BEACONS:{
        bcb = new S2BeaconsBehaviour(this);
    }break;
    case VSMSubsystems::S1_BEACONS:{
        cout<<"changing s1 type to BEACON\n";
        bcb = new BeaconListenerBehaviour(this);

    }break;
    case VSMSubsystems::BEACON_MASTER:{
        bcb = new BeaconMasterBehaviour(this);
        uwbMsgListener.idFromBeaconType=(uint8_t)Topics::BEACON_MASTER_IN;

    }break;
    case VSMSubsystems::S1_EXPLORERS:{
        cout<<"changing s1 type to EXPLORER\n";
        bcb = new ExplorerListenerBehaviour(this);
    }break;
    case VSMSubsystems::S2_EXPLORERS:{
        bcb = new S2ExplorersBehaviour(this);

    }break;
    case VSMSubsystems::S4:{
        bcb = new S4Behaviour(this);

    }break;
    case VSMSubsystems::TARGET_MOVING_BEACON:{
        bcb = new MoveToTargetBehaviour(this);

    }break;
    default:{
        cout<<"not implemented \n";
        return;
    }
    }

    behavioursList.push_back(bcb);
    if(behaviour==VSMSubsystems::S3) isS3 = true; //todo - hardcoded name
    string behName =typeid(*bcb).name();
    cout<<std::setprecision (15)<< getSystemTimeSec()<<" added behaviour: "<<behName<<"\n";

}

void RoombaAgent::removeBehaviour(string name)// invalid, enum used instead of name
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
    roleList.erase(std::remove(roleList.begin(),roleList.end(),bcb->type),roleList.end());// remove from enums list, but is this list needed?

    behavioursList.erase(std::remove(behavioursList.begin(), behavioursList.end(), bcb), behavioursList.end());// remove by value
    cout<< getSystemTimeSec()<<" ra removing behaviour: "<<typeid (*bcb).name()<<"\n";
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

       // cout<<"ra send to rec: " <<receiver<< "from s "<<(int)msg.sender<<"\n";



    std::set<BaseCommunicationBehaviour*> subs = subscribersMap.at(receiver);
    std::set<BaseCommunicationBehaviour*>::iterator it;

    for (it = subs.begin(); it != subs.end(); ++it) {
        (*it)->msgDeque.push_back(new VSMMessage(&msg));
    }



    // send message to others
    uwbMsgListener.addToTxDeque(msg);//change to  pass by reference?

}

BaseCommunicationBehaviour *RoombaAgent::findBehaviourByName(VSMSubsystems name)// astually find by type
{
    for (int i = 0; i < behavioursList.size(); ++i) {
        string bName = (typeid(behavioursList.at(i)).name());

        if(behavioursList.at(i)->type==name){
            return behavioursList.at(i);
        }
    }
    return 0;
}

BaseCommunicationBehaviour *RoombaAgent::findBehaviourByType(VSMSubsystems type)
{

}

double RoombaAgent::getSystemTimeSec(void){
    struct timeval start_time;
    double milli_time;
    gettimeofday(&start_time, NULL);

    milli_time = ((start_time.tv_usec) / 1000000.0 + start_time.tv_sec);
    return milli_time;
}

void RoombaAgent::setS1Type(VSMSubsystems type)
{
    if(type==VSMSubsystems::S1_BEACONS || type==VSMSubsystems::S1_EXPLORERS ){
        addBehaviour(type);
        s1Type= type;

    }
    }
