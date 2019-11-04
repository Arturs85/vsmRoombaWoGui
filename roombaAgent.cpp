#include "roombaAgent.hpp"


void RoombaAgent::initHardware(){
    uwbMsgListener.initialize();
    uwbMsgListener.startReceiving();
    uwbMsgListener.startSending();

    uartTest.initialize();
    uartTest.startReceiveing();//starts receiving and sending threads
    roombaController=new RoombaController(&uartTest);
    roombaController->startFull();
}
