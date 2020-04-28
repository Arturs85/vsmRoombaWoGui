#include "roombaMovementManager.hpp"
#include <iostream>
#include <ctime>

RoombaController* RoombaMovementManager::roombaController=0;
//LocalMap* RoombaMovementManager::localMap=0;
pthread_mutex_t RoombaMovementManager::mutexGUIData = PTHREAD_MUTEX_INITIALIZER;


bool RoombaMovementManager::isRunning=true;
bool RoombaMovementManager::isDriving=false;
int RoombaMovementManager::distanceRemaining =0;
int RoombaMovementManager::angleRemaining =0;
int RoombaMovementManager::direction =0;
int RoombaMovementManager::odometry =0;

MovementStates RoombaMovementManager::state= MovementStates::IDLE; 

RoombaMovementManager::RoombaMovementManager(RoombaController *roombaController)
{
    RoombaMovementManager::roombaController = roombaController;
    //    RoombaMovementManager::localMap= localMap;
    roombaController->readAngle();
    roombaController->readDistance();// to reset roomba counters
    startThread();
}

RoombaMovementManager::~RoombaMovementManager()
{
}

bool RoombaMovementManager::driveDistance(int distMm)
{
    if(state==MovementStates::DRIVING||state==MovementStates::TURNING_LEFT||state ==MovementStates::TURNING_RIGHT) return false;// dont let start next movement until previous is finished

    roombaController->readDistance();// this clears roomba internal counter
    distanceRemaining = distMm;
    state = MovementStates::DRIVING;
    roombaController->driveForward();
}

bool RoombaMovementManager::turnLeft(int degrees)
{
    if(state==MovementStates::DRIVING||state==MovementStates::TURNING_LEFT||state ==MovementStates::TURNING_RIGHT) return false;// dont let start next movement until previous is finished

    roombaController->readAngle();// this clears roomba internal counter
    angleRemaining = degrees;
    state = MovementStates::TURNING_LEFT;
    roombaController->turnLeft();//
}

bool RoombaMovementManager::turnRight(int degrees)
{
    if(state==MovementStates::DRIVING||state==MovementStates::TURNING_LEFT||state ==MovementStates::TURNING_RIGHT) return false;// dont let start next movement until previous is finished

    roombaController->readAngle();// this clears roomba internal counter
    angleRemaining = degrees;
    state = MovementStates::TURNING_RIGHT;
    roombaController->turnRight();//

}

bool RoombaMovementManager::turn(int degrees)
{
    if(degrees<0)
        turnRight(-degrees);
    else
        turnLeft(degrees);
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
        case MovementStates::DRIVING:{
            int16_t dist = roombaController->readDistance();// is right units used - mm ?
            //distanceRemaining -= dist;
            distanceRemaining += (dist*7.7);//roomba koefic to get mm from roomba value
            odometry+=abs(dist*7.7);// to track total odometry
            // std::cout<<"rmm distRemain: "<<distanceRemaining<<"\n";

            if(distanceRemaining<0){
                state = MovementStates::FINISHED;
                roombaController->stopMoving();
            }}
            break;
        case MovementStates::TURNING_LEFT:{
            int16_t angle = 3*roombaController->readAngle();
            angleRemaining -= angle;// for knowing when to stop
            std::cout<<"===--->> rmm angRemain: "<<angleRemaining<<"\n";
            direction+= angle;// for tracking direction

            if(angleRemaining<0){
                roombaController->stopMoving();
                state = MovementStates::FINISHED;
            }}
            break;
        case MovementStates::TURNING_RIGHT:{
            int16_t angle = 3*roombaController->readAngle();
            angleRemaining += angle;// for knowing when to stop
            std::cout<<"===--->> rmm angRemain: "<<angleRemaining<<"\n";
            direction+= angle;// for tracking direction

            if(angleRemaining<0){
                roombaController->stopMoving();
                state = MovementStates::FINISHED;
            }}
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
