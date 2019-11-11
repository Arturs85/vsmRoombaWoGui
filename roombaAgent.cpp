#include "roombaAgent.hpp"
#include <unistd.h>
#include <limits.h>

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
    roombaController=new RoombaController(&uartTest);
    roombaController->startFull();
}

RoombaAgent::RoombaAgent()
{
    getId();
    roleList.push_back(VSMSubsystems::S1);//for test
    roleList.push_back(VSMSubsystems::S3);//for test
getRoleListForMsg();
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
