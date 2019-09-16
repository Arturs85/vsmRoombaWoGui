#include "roombaBehaviour.hpp"
#include <iostream>
#include <ctime>

RoombaController* RoombaBehaviour::roombaController=0;
LocalMap* RoombaBehaviour::localMap=0;
UwbMsgListener* RoombaBehaviour::uwbMsgListener =0;
pthread_mutex_t RoombaBehaviour::mutexGUIData = PTHREAD_MUTEX_INITIALIZER;


bool RoombaBehaviour::isRunning=true;
LogFileSaver* RoombaBehaviour::logFileSaver=0;


RoombaBehaviour::RoombaBehaviour(RoombaController *roombaController,LocalMap* localMap,UwbMsgListener* uwbMsgListener)
{
    logFileSaver = new LogFileSaver();
    RoombaBehaviour::roombaController = roombaController;
    RoombaBehaviour::localMap= localMap;
RoombaBehaviour::uwbMsgListener = uwbMsgListener;
    startThread();
}

RoombaBehaviour::~RoombaBehaviour()
{
    if(logFileSaver!=0)
        logFileSaver->closeFile();
delete(logFileSaver);
}

void RoombaBehaviour::startThread()
{
    int iret1 = pthread_create( &roombaBehaviourThread, NULL, behaviourLoop, 0);

    if(iret1)
    {
        fprintf(stderr,"Error - RoombaBehaviour pthread_create() return code: %d\n",iret1);

    }
    else{
        cout<<"roomba Behaviour thread started\n";

    }
}

void *RoombaBehaviour::behaviourLoop(void *arg)
{

    while(isRunning){
        uint8_t lb = roombaController->readLightBumps();
        uint8_t bwd = roombaController->readBumpsnWheelDrops();
        int16_t dist = roombaController->readDistance();
        int16_t angle = roombaController->readAngle();

        uint16_t ca = roombaController->readBattCapacity();
        uint16_t ch = roombaController->readBattCharge();

            long int t = static_cast<long int> (time(NULL));
            logFileSaver->writeEntry(LogEntry(t,dist,angle,lb));

        localMap->bat=(100*ch/++ca);

        localMap->lightBumps=lb;
        localMap->addObstacles(lb);
        localMap->updateObstaclePosition(dist,angle);
if(uwbMsgListener->rxDeque.size()>0){
VSMMessage msg = uwbMsgListener->rxDeque.back();
if(msg.paramName=="stop")
roombaController->stopMoving();
uwbMsgListener->rxDeque.pop_back();
}

        //----------driving----------------

        //        uint8_t bwd = roombaController->readBumpsnWheelDrops();

//        if(lb!=0){
//            usleep(1000);
//            lb = roombaController->readLightBumps();
//            if(lb!=0)//double check
//            {
//                roombaController->drive(50,1);
//                usleep(1000000);
//            }


//        }else
//        {
//            roombaController->drive(50,32767);
//        }

        //----------drivingEnd----------------

        //localMap->Refresh();
        usleep(250000);
    }
}
