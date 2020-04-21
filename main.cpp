
#include "uartTest.h"
#include "uwbmsglistener.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <linux/input.h>
#include <sstream>
#include <vector>
#include "roombaController.hpp"
#include "roombaBehaviour.hpp"
#include "localMap.hpp"
#include "roombaAgent.hpp"
#include <pthread.h>
#include <signal.h>

class MyApp
{
public:
   static void* OnInit(void* arg);
    pthread_t agentThread;

static RoombaAgent* roombaAgent;
    LocalMap* localMap;
static void intHandler(int dummy); 
public:
    int main(int ac,char**av);

};
int main(int ac,char**av){
    MyApp myapp;
    myapp.main(0,0);
}
RoombaAgent* MyApp::roombaAgent = new RoombaAgent();

void MyApp::intHandler(int dummy) {
    if(roombaAgent!=0){
		if(roombaAgent->roombaController!=0){
			roombaAgent->roombaController->shutDown();
		     usleep(1000000);
		}    
	}	
    exit(0);
}


int MyApp::main(int argc, char** av)
{
	 signal(SIGINT, intHandler);
  RoombaAgent roombaAgent=*this->roombaAgent;
    pthread_t threadId;

    // Create a thread that will function threadFunc()
    int err = pthread_create(&threadId, NULL, &OnInit, NULL);
    // Check if thread is created sucessfuly
    if (err)
    {
        std::cout << "agent thread creation failed : " << strerror(err);
        return err;
    }
    else
        std::cout << "agent thread Created with ID : " << threadId << std::endl;


    printf("main started\n");
    //OnInit();
    std::string tmp = "test data";
    char *arr = &tmp[0];

    //uartTest.setDataToTransmit(arr, 9);
    //keyboard reading
    cout<<"OnInit compleate\n";
    while(1){
        std::string command;
        getline (cin, command);

        if(!command.compare("raw")){
            std::string mystr;
            printf( "enter chars!\n");
            getline (cin, mystr);
            vector<uint8_t> comm;


            std::istringstream ss(mystr);
            cout<<"you entered: "<<mystr;

            while(!ss.eof())// read integers from string
            {
                int i;
                if(ss>>i)
                {
                    comm.push_back(i);
                    cout<< i<<" ";
                }
            }

            cout<<"\n";

            char *ar = &mystr[0];
            //printf("first char you entered: %d", (uint8_t)(*ar));
            //uartTest.setDataToTransmit(ar, mystr.size());
            roombaAgent.uartTest.setDataToTransmit(comm);

            if(comm[0]==142 && comm[1]==25){
                vector<uint8_t> ch = roombaAgent.uartTest.readNumberOfBytes(2);
                cout<<"vector ch size: "<<ch.size()<<"\n";
                if(ch.size()==2)
                {//convertData

                    uint16_t* batCharge = reinterpret_cast<uint16_t*>(&ch[0]);
                    cout<<"bat charge received: "<<*batCharge<<"mAh\n";
                }
            }

            else if(comm[0]==142 && comm[1]==26){
                vector<uint8_t> ch = roombaAgent.uartTest.readNumberOfBytes(2);
                cout<<"vector ch size: "<<ch.size()<<"\n";
                if(ch.size()==2)
                {//convertData

                    uint16_t* batCapacity = reinterpret_cast<uint16_t*>(&ch[0]);
                    cout<<"bat capacity received: "<<*batCapacity<<"mAh\n";
                }
            }

            comm.clear();
        }
        else if(!command.compare("bat")){
            uint16_t ca = roombaAgent.roombaController->readBattCapacity();
            uint16_t ch = roombaAgent.roombaController->readBattCharge();

            cout<<"batt ca: "<<ca<< ", ch: "<<ch<<" left: "<<(100*ch/++ca)<<" %\n";
        }
        else if(!command.compare("turn r")){
            roombaAgent.roombaController->drive(50,-1);
        }
        else if(!command.compare("stop")){
           // roombaBehaviour->isRunning=false;
            roombaAgent.roombaController->drive(0,0);
        }
        else if(!command.compare("lastdist")){
            int16_t dist = roombaAgent.roombaController->readDistance();
            cout<<"distance: "<<dist;
        }
        else if(!command.compare("angle")){
            int16_t ang = roombaAgent.roombaController->readAngle();
            cout<<"angle: "<<ang;
        }
        else if(!command.compare("lbumps")){
            uint8_t lb = roombaAgent.roombaController->readLightBumps();
            cout<<"light bumps: "<<lb;
        }
        else if(!command.compare("advert")){
           roombaAgent.advertise();
        }
        else if(!command.compare("roam")){
            //  roombaBehaviour = new RoombaBehaviour(&roombaController,localMap);
        }
        else if(!command.compare("end")){
            //  roombaBehaviour = new RoombaBehaviour(&roombaController,localMap);
            //exit_handler(0);
        }
        else if(!command.compare("dist"))
        {
            int nr=0;
            printf( "enter measurement target nr!\n");
            cin>>nr;
            cout<<"target: "<<nr<<"/n";

            roombaAgent.uwbMsgListener.addToRangingInitDeque(nr);
        }

        else if(command.find("send")!=std::string::npos)
        {
            roombaAgent.uwbMsgListener.addToTxDeque(command.substr(4,std::string::npos));
        }

    }
    roombaAgent.uartTest.waitUartThreadsEnd();

    return 0;
}





void* MyApp::OnInit(void* arg)
{
roombaAgent->initHardware();
roombaAgent->startCycle();
//    uwbMsgListener.initialize();
//    uwbMsgListener.startReceiving();
//    uwbMsgListener.startSending();

//    uartTest.initialize();
//    uartTest.startReceiveing();//starts receiving and sending threads
//    roombaController=new RoombaController(&uartTest);

//    localMap = new LocalMap();
//    roombaBehaviour = new RoombaBehaviour(roombaController,localMap,&uwbMsgListener);



//    roombaController->startFull();

//    roombaController->sevenSegmentDisplay(65);// use display as ON indicator


}

