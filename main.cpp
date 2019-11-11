
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



class MyApp
{
public:
    bool OnInit();


    LocalMap* localMap;
    UartTest uartTest;
    static RoombaBehaviour* roombaBehaviour;
    static RoombaController* roombaController;
    UwbMsgListener uwbMsgListener; // start uwb device

public:
    int main(int ac,char**av);

};
int main(int ac,char**av){
    MyApp myapp;
    myapp.main(0,0);
}

RoombaController* MyApp::roombaController=0;
RoombaBehaviour* MyApp::roombaBehaviour=0;

void exit_handler(int s){
    printf("Caught signal %d\n",s);
    MyApp::roombaController->shutDown();
    if(MyApp::roombaBehaviour!=0)
        delete(MyApp::roombaBehaviour);
    // uartTest.waitUartThreadsEnd();
    usleep(100000);
    exit(1);

}
int MyApp::main(int argc, char** av)
{

    printf("main started\n");
    OnInit();
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
            uartTest.setDataToTransmit(comm);

            if(comm[0]==142 && comm[1]==25){
                vector<uint8_t> ch = uartTest.readNumberOfBytes(2);
                cout<<"vector ch size: "<<ch.size()<<"\n";
                if(ch.size()==2)
                {//convertData

                    uint16_t* batCharge = reinterpret_cast<uint16_t*>(&ch[0]);
                    cout<<"bat charge received: "<<*batCharge<<"mAh\n";
                }
            }

            else if(comm[0]==142 && comm[1]==26){
                vector<uint8_t> ch = uartTest.readNumberOfBytes(2);
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
            uint16_t ca = roombaController->readBattCapacity();
            uint16_t ch = roombaController->readBattCharge();

            cout<<"batt ca: "<<ca<< ", ch: "<<ch<<" left: "<<(100*ch/++ca)<<" %\n";
        }
        else if(!command.compare("turn r")){
            roombaController->drive(50,-1);
        }
        else if(!command.compare("stop")){
            roombaBehaviour->isRunning=false;
            roombaController->drive(0,0);
        }
        else if(!command.compare("lastdist")){
            int16_t dist = roombaController->readDistance();
            cout<<"distance: "<<dist;
        }
        else if(!command.compare("angle")){
            int16_t ang = roombaController->readAngle();
            cout<<"angle: "<<ang;
        }
        else if(!command.compare("lbumps")){
            uint8_t lb = roombaController->readLightBumps();
            cout<<"light bumps: "<<lb;
        }
        else if(!command.compare("roam")){
            //  roombaBehaviour = new RoombaBehaviour(&roombaController,localMap);
        }
        else if(!command.compare("end")){
            //  roombaBehaviour = new RoombaBehaviour(&roombaController,localMap);
            exit_handler(0);
        }
        else if(!command.compare("dist"))
        {
            int nr=0;
            printf( "enter measurement target nr!\n");
            cin>>nr;
            cout<<"target: "<<nr<<"/n";

            uwbMsgListener.addToRangingInitDeque(nr);
        }

        else if(command.find("send")!=std::string::npos)
        {
            uwbMsgListener.addToTxDeque(command.substr(4,std::string::npos));
        }

    }
    uartTest.waitUartThreadsEnd();

    return 0;
}





bool MyApp::OnInit()
{



    uwbMsgListener.initialize();
    uwbMsgListener.startReceiving();
    uwbMsgListener.startSending();

    uartTest.initialize();
    uartTest.startReceiveing();//starts receiving and sending threads
    roombaController=new RoombaController(&uartTest);

    localMap = new LocalMap();
    roombaBehaviour = new RoombaBehaviour(roombaController,localMap,&uwbMsgListener);



    roombaController->startFull();

    roombaController->sevenSegmentDisplay(65);// use display as ON indicator

    return true;
}

