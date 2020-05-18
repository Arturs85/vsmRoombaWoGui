
#ifndef UWBMSGLISTENER_HPP
#define UWBMSGLISTENER_HPP

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <cstring>
#include <string>
#include <sstream>      // std::stringstream
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <termios.h>
#include <deque>
#include <vector>
extern "C" {
#include "deca_device_api.h"
#include "deca_regs.h"
#include "platform.h"
#include "vSMMessage.hpp"
}
using namespace std;


typedef unsigned long long uint64;
class RoombaAgent;
class UwbMsgListener
{

public:
    static int uart0_filestream;
    pthread_t receivingThreadUwb;
    pthread_t sendingThreadUwb;
    static pthread_mutex_t txBufferLock;
    static pthread_mutex_t rangingInitBufferLock;
    static pthread_mutex_t dwmDeviceLock;
    static pthread_mutex_t rxDequeLock;

    //static pthread_mutex_t mutexSend;
    static char tx_buffer[];
    static int tx_size;//Unused?
    static struct termios orig_termios;
    static bool isSending;
    static bool isReceivingThreadRunning;
    static uint8_t idFromHostname;
    static uint8_t idFromBeaconType;

    //size_t rawTxMessageNominalSize = 150;// header+127=?
    static std::deque<RawTxMessage> txDeque;
    static std::deque<VSMMessage> rxDeque;
    static std::deque<int> rangingInitDeque;
    static RoombaAgent* owner;

    UwbMsgListener(RoombaAgent* owner);
    UwbMsgListener();

    ~UwbMsgListener();
    static void initialize(char hostId);
    static void* receivingLoop(void* arg);

    void startReceiving();
    void startSending();
    void stopSending();
    static void readDeviceData();


    static void* receive(void* arg);
    static void addToTxDeque(std::string msgText);
    static void addToTxDeque(VSMMessage msg);

    static void addToRangingInitDeque(int rangingTarget);
    static void respondToRangingRequest(uint8_t initiatorId,uint8_t ownId);
    static void initiateRanging(int targetId=0);
    static VSMMessage *getFirstRxMessageFromDeque();//and remove it from deque

    //static void final_msg_get_ts(uint8 *ts_field, uint64 ts);


    static void* sendingLoop(void* arg);
    void waitUwbThreadsEnd();

    static void set_conio_terminal_mode();
    static void reset_terminal_mode();
    static int kbhit();
};






#endif //UWBMSGLISTENER

