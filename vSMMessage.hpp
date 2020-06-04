#ifndef VSMMESSAGE_HPP
#define VSMMESSAGE_HPP
#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>      // std::stringstream

#define DIRECT_ADRESS_MAX 30

enum class VSMSubsystems{NONE=DIRECT_ADRESS_MAX,S1,S1_BEACONS,S1_EXPLORERS,S2_BEACONS,S2_EXPLORERS,S3,S4,ROLE_CHECKING,BEACON_ONE,BEACON_TWO,
                         BEACON_MASTER,TARGET_MOVING_BEACON};
enum class Topics{NONE=DIRECT_ADRESS_MAX,BROADCAST,S1_,S2BEACONS_IN,S3_IN,S4_,BEACON_IN,S3_ROLE_CHECKING,CVAL_TO_S2,CVAL_INC_REQUEST,TWO_POINT_FORMATION_TO_MOVING,
                  TWO_POINT_FORMATION_TO_STILL,S2_TO_BEACONS,TO_S2BEACONS,THIRD_BEACON_OUT,THIRD_BEACON_IN,S1_REQUESTS,
                  BEACON_MASTER_IN,BEACON_ONE_IN,BEACON_TWO_IN,S2EXPLORERS_IN, S2_TO_EXPLORERS,TO_S4, SIZE_OF_THIS_ENUM};

enum class MessageContents{NONE,Test,DISTANCE_MEASUREMENT,ROLE_CHECK_WITH_S3,S3REPLY_TO_ROLE_CHECK,TPF_START,AGREE,FIRST_FORMATION_START,FORMATION_COMPLETED,QUERRY_INFO,
                          BEACON_ROLE,CONFIRM_ROLE,THIRD_BFP_START,TPFP_DONE,ACKNOWLEDGE,REQUEST_S1,S1_CHANGE_TYPE,DISTANCE_MEASUREMENT_AND_ID,
                          CORDINATES_XY,START_EXPLORING, CVALUES,BEACONS_RQ,STOP_EXPLORING,EXPLORING_DONE,MOVE_TO_TARGET,AGREE_MOVE_TO_TARGET,TARGET_RECEIVED  };
//enum S1Types{BEACON,EXPOLRER};

typedef struct RawTxMessage {char macHeader[10]; char data[127]; int dataLength; } RawTxMessage;

using namespace std;

class VSMMessage{
public:
    VSMSubsystems sender=VSMSubsystems::NONE;
    Topics receiver=Topics::NONE;
    int receiverNumber=0;
    int senderNumber=0;
    MessageContents contentDescription= MessageContents::NONE;
    std::string content;


    VSMMessage():contentDescription(MessageContents::NONE),content(""),sender(VSMSubsystems::NONE),receiver(Topics::NONE){}
    VSMMessage(VSMSubsystems sender, Topics receiver,    MessageContents contentDescription,std::string content):contentDescription(contentDescription),content(content),sender(sender),receiver(receiver){}
    VSMMessage(int sender, Topics receiver,    MessageContents contentDescription,std::string content):contentDescription(contentDescription),content(content),senderNumber(sender),receiver(receiver){}
    VSMMessage(VSMSubsystems sender, int receiver,    MessageContents contentDescription,std::string content):contentDescription(contentDescription),content(content),sender(sender),receiverNumber(receiver){}
    VSMMessage(int sender, int receiver,    MessageContents contentDescription,std::string content):contentDescription(contentDescription),content(content),senderNumber(sender),receiverNumber(receiver){}

    VSMMessage(VSMMessage* old){
        this->contentDescription = old->contentDescription;
        this->content = old->content;
        this->receiver=old->receiver;
        this->sender = old->sender;
        this->receiverNumber= old->receiverNumber;
        this->senderNumber= old->senderNumber;
    }
   VSMMessage(const VSMMessage &old){
        this->contentDescription = old.contentDescription;
        this->content = old.content;
        this->receiver=old.receiver;
        this->sender = old.sender;
        this->receiverNumber= old.receiverNumber;
        this->senderNumber= old.senderNumber;
    }
    void setSender(int sender){senderNumber = sender;}
    void setSender(VSMSubsystems sender){this->sender = sender;}
    void setReceiver(int receiver){receiverNumber = receiver;}
    void setReceiver(Topics receiver){this->receiver = receiver;}


    std::string toString(){
    int sender;
        if(this->sender==VSMSubsystems::NONE)
        sender=senderNumber;
        else
        sender =(int)this->sender;

        int receiver;
            if(this->receiver==Topics::NONE)
            receiver=receiverNumber;
            else
            receiver =(int)this->receiver;


        return to_string(sender)+","+to_string(receiver)+","+to_string((int)contentDescription)+","+content;
        //VSMMessage()
    }
    static int stringToVsmMessage(std::string msgStr, VSMMessage *destMsg){
        vector<std::string> msgFields = split(msgStr);
        if(msgFields.size()!=4) return 0;
        try{

            int sender = stoi(msgFields[0]);
            if(sender<=DIRECT_ADRESS_MAX)//this is hostname number based adress of the unit
                destMsg->senderNumber = sender;
            else
                destMsg->sender =static_cast<VSMSubsystems>(sender);

            int receiver = stoi(msgFields[1]);
            if(receiver<=DIRECT_ADRESS_MAX)
                destMsg->receiverNumber = receiver;
            else
                destMsg->receiver =static_cast<Topics>(receiver);
        }catch(std::invalid_argument){
            std::cout<<"stoi error: "<<msgFields[0]<<"\n";
            return 0;
        }

        destMsg->contentDescription = static_cast<MessageContents>(stoi(msgFields[2]));
        destMsg->content = msgFields[3];
        return 1;
    }
    static vector<std::string> split(string src,char delimChar=','){
        vector<std::string> result;
        stringstream ss(src);
        while( ss.good() )
        {
            string substr;
            getline( ss, substr, delimChar );
            result.push_back( substr );
        }
        return result;
    }

};





#endif //VSMMESSAGE_HPP
