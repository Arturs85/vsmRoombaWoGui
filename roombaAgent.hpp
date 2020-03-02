#ifndef ROOMBAAGENT_HPP
#define ROOMBAAGENT_HPP

#include "roombaController.hpp"
#include "uwbmsglistener.hpp"
#include "uartTest.h"
#include "baseCommunicationBehaviour.hpp"
#include <map>
//class BaseCommunicationBehaviour;
class RoombaAgent{

public:
RoombaController* roombaController;
UwbMsgListener uwbMsgListener; // start uwb device
UartTest uartTest;
vector<VSMSubsystems> roleList;
vector<BaseCommunicationBehaviour*> behavioursList;
vector<vector<BaseCommunicationBehaviour*>> subscribersMap; // index of vector- topic ordinal, vector of Behaviors- subscribers whom to deliver msg
std::map<std::string,vector<std::string > > conflictingBehaviours;
bool isS3=false;

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

private:
void startCycle();//
void behavioursStep();
void addBehaviour(BaseCommunicationBehaviour *bcb);
void removeBehaviour(std::string name);

BaseCommunicationBehaviour* findBehaviourByName(string name);
};

#endif //ROOMBAAGENT_HPP
