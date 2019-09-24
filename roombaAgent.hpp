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

void initHardware();

private:





};

#endif //ROOMBAAGENT_HPP
