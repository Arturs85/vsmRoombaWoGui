#ifndef ROOMBAAGENT_HPP
#define ROOMBAAGENT_HPP

#include "roombaController.hpp"
#include "uwbmsglistener.hpp"
#include "uartTest.h"


class RoombaAgent{

public:
RoombaController* roombaController;
UwbMsgListener uwbMsgListener; // start uwb device
UartTest uartTest;
vector<VSMSubsystems> roleList;
char id=0;
std::string getRoleListForMsg();
void initHardware();
RoombaAgent();
void getId();

private:





};

#endif //ROOMBAAGENT_HPP
