
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

}
using namespace std;
enum VSMSubsystems{S1,S2,S3,S4};
typedef struct RawTxMessage {char macHeader[10]; char data[127]; int dataLength; } RawTxMessage;
typedef struct VSMMessage {VSMSubsystems sender; VSMSubsystems receiver;string paramName;float paramValue;
                           string toString(){
                               return to_string((int)sender)+","+to_string((int)receiver)+","+paramName+","+to_string(paramValue);
                               //VSMMessage()
                           }
                                                  static int stringToVsmMessage(std::string msgStr, VSMMessage *destMsg){
                                                      vector<string> msgFields = split(msgStr);
                                                      if(msgFields.size()!=4) return 0;
                                                      try{
                                                      destMsg->sender =static_cast<VSMSubsystems>(stoi(msgFields[0]));
                                                      destMsg->receiver =static_cast<VSMSubsystems>(stoi(msgFields[1]));
												  }catch(std::invalid_argument){
													  cout<<"stoi error: "<<msgFields[0]<<"\n";
													  return 0;
													  }
                                                      destMsg->paramName = msgFields[2];
                                                      destMsg->paramValue = stof(msgFields[3]);
                                                      return 1;
                                                  }
                                                                         static vector<string> split(string src,char delimChar=','){
                                                                             vector<string> result;
                                                                             stringstream ss(src);
                                                                             while( ss.good() )
                                                                             {
                                                                                 string substr;
                                                                                 getline( ss, substr, delimChar );
                                                                                 result.push_back( substr );
                                                                             }
                                                                             return result;
                                                                         }

                          } VSMMessage;

typedef unsigned long long uint64;

class UwbMsgListener
{

public:
    static int uart0_filestream;
    pthread_t receivingThreadUwb;
    pthread_t sendingThreadUwb;
    static pthread_mutex_t txBufferLock;
    static pthread_mutex_t rangingInitBufferLock;

    static pthread_mutex_t dwmDeviceLock;

    //static pthread_mutex_t mutexSend;
    static char tx_buffer[];
    static int tx_size;//Unused?
    static struct termios orig_termios;
    static bool isSending;
    static bool isReceivingThreadRunning;
    //size_t rawTxMessageNominalSize = 150;// header+127=?
    static std::deque<RawTxMessage> txDeque;
    static std::deque<VSMMessage> rxDeque;
    static std::deque<int> rangingInitDeque;


    UwbMsgListener();
    ~UwbMsgListener();
    static void initialize();
    static void* receivingLoop(void* arg);

    void startReceiving();
    void startSending();
    void stopSending();
    static void readDeviceData();


    static void* receive(void* arg);
    static void addToTxDeque(std::string msgText);
    static void addToRangingInitDeque(int rangingTarget);
    static void respondToRangingRequest();
    static void initiateRanging();

    //static void final_msg_get_ts(uint8 *ts_field, uint64 ts);


    static void* sendingLoop(void* arg);
    void waitUwbThreadsEnd();

    static void set_conio_terminal_mode();
    static void reset_terminal_mode();
    static int kbhit();
};






#endif //UWBMSGLISTENER

