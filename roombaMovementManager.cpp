#include "roombaMovementManager.hpp"
#include <iostream>
#include <ctime>

RoombaController* RoombaMovementManager::roombaController=0;
//LocalMap* RoombaMovementManager::localMap=0;
pthread_mutex_t RoombaMovementManager::mutexGUIData = PTHREAD_MUTEX_INITIALIZER;


bool RoombaMovementManager::isRunning=true;
bool RoombaMovementManager::isDriving=false;
int RoombaMovementManager::distanceRemaining =0;


RoombaMovementManager::RoombaMovementManager(RoombaController *roombaController)
{
    RoombaMovementManager::roombaController = roombaController;
    //    RoombaMovementManager::localMap= localMap;
    startThread();
}

RoombaMovementManager::~RoombaMovementManager()
{
}

bool RoombaMovementManager::driveDistance(int distMm)
{
    if(state==MovementStates::DRIVING||state==MovementStates::TURNING) return false;// dont let start next movement until previous is finished

    roombaController->readDistance();// this clears roomba internal counter
    distanceRemaining = distMm;
    state = MovementStates::DRIVING;
    roombaController->driveForward();
}

bool RoombaMovementManager::turnLeft(int degrees)
{
    if(state==MovementStates::DRIVING||state==MovementStates::TURNING) return false;// dont let start next movement until previous is finished

    roombaController->readAngle();// this clears roomba internal counter
    angleRemaining = degrees;
    state = MovementStates::TURNING;
    roombaController->turnLeft();//
}

void RoombaMovementManager::startThread()
{
    int iret1 = pthread_create( &roombaMovementManagerThread, NULL, behaviourLoop, 0);

    if(iret1)
    {
        fprintf(stderr,"Error - roomba movement manager pthread_create() return code: %d\n",iret1);

    }
    else{
        cout<<"roomba movement manager thread started\n";

    }
}

void *RoombaMovementManager::behaviourLoop(void *arg)
{

    while(isRunning){
        uint8_t lb = roombaController->readLightBumps();
        uint8_t bwd = roombaController->readBumpsnWheelDrops();


        uint16_t ca = roombaController->readBattCapacity();
        uint16_t ch = roombaController->readBattCharge();

        long int t = static_cast<long int> (time(NULL));

        switch (state) {
        case MovementStates::DRIVING:
            int16_t dist = roombaController->readDistance();// is right units used - mm ?
            distanceRemaining -= dist;
            if(distanceRemaining<0){
                state = MovementStates::FINISHED;
                roombaController->stopMoving();
            }
            break;
        case MovementStates::TURNING:
            int16_t angle = roombaController->readAngle();
            angleRemaining -= angle;// for knowing when to stop
            direction+= angle;// for tracking direction

            if(angleRemaining<0){
                roombaController->stopMoving();
                state = MovementStates::FINISHED;
            }
            break;
        case MovementStates::FINISHED:
            break;
        }


        usleep(250000);
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
