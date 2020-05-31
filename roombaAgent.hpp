#ifndef ROOMBAAGENT_HPP
#define ROOMBAAGENT_HPP

#include "roombaController.hpp"
#include "uwbmsglistener.hpp"
#include "uartTest.h"
#include "baseCommunicationBehaviour.hpp"
#include <map>
#include <set>
//class BaseCommunicationBehaviour;
class RoombaMovementManager;
class RoombaAgent{

public:
RoombaController* roombaController;
UwbMsgListener uwbMsgListener; // start uwb device
UartTest uartTest;
vector<VSMSubsystems> roleList;
std::vector<BaseCommunicationBehaviour*> behavioursList;
vector<set<BaseCommunicationBehaviour*>> subscribersMap; // index of vector- topic ordinal, vector of Behaviors- subscribers whom to deliver msg
std::map<VSMSubsystems,vector<VSMSubsystems > > conflictingBehaviours;
bool isS3=false;
VSMSubsystems s1Type=VSMSubsystems::S1_BEACONS;//default s1type of robot

void addBehaviourToList(VSMSubsystems behaviour);
vector<VSMSubsystems> rolesToAdd;

char id=0;
std::string getRoleListForMsg();
void initHardware();
RoombaAgent();
void getId();
void advertise();
void distributeMessages();
bool isRunning = true;
void removeBehaviour(BaseCommunicationBehaviour *bcb);
void sendMsg(VSMMessage msg);
void startCycle();//
void addBehaviour(VSMSubsystems behaviour);
RoombaMovementManager* movementManager;
static double getSystemTimeSec();
void setS1Type(VSMSubsystems type);// use only valid s1 types
private:
double lastTime=0;
void behavioursStep();
void removeBehaviour(std::string name);

BaseCommunicationBehaviour* findBehaviourByName(VSMSubsystems name);
};

#endif //ROOMBAAGENT_HPP
