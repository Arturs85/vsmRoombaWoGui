
#ifndef ROOMBAMOVEMENTMANAGER_HPP
#define ROOMBAMOVEMENTMANAGER_HPP
#include <stdio.h>
#include <pthread.h>
#include <cstring>
#include <vector>
#include <stdint.h>
#include "roombaController.hpp"


enum class MovementStates{IDLE,FINISHED,DRIVING,TURNING_LEFT,TURNING_RIGHT};
using namespace std;

class RoombaMovementManager
{
public:
    RoombaMovementManager(RoombaController* roombaController);
    ~RoombaMovementManager();
    static bool isRunning;

    static bool driveDistance(int distMm);
    static bool turnLeft(int degrees);
    static bool turnRight(int degrees);
    static bool turn(int degrees);//turn by amount of degrees, direction determined by sign

    static int distanceRemaining;
    static int angleRemaining;
static int direction;//tracks direction relative to starting point
static int odometry;//tracks travel from starting point

static bool isObstacleDetected;//set if driving was inhibited by obstacle
static void stopMotion();

static MovementStates state;
private:
    static bool isDriving;
    static RoombaController* roombaController;
   // static LocalMap* localMap;
    pthread_t roombaMovementManagerThread;
    static pthread_mutex_t mutexGUIData;

    void startThread();
    static void* behaviourLoop(void* arg);
static uint8_t bumperInitialstate;
};






#endif //ROOMBAMOVEMENTMANAGER_HPP

