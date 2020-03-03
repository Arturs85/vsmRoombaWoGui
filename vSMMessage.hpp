#ifndef VSMMESSAGE_HPP
#define VSMMESSAGE_HPP
#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>      // std::stringstream

enum VSMSubsystems{S1,S2,S3,S4};
enum Topics{BROADCAST,S1_,S2_,S3_,S4_,BEACON_IN,SIZE_OF_THIS_ENUM};
enum MessageContents{Test,DISTANCE_MEASUREMENT,RoleCheckWithS3};
enum Behaviours{BEACON};

typedef struct RawTxMessage {char macHeader[10]; char data[127]; int dataLength; } RawTxMessage;

using namespace std;

class VSMMessage{
public:
    VSMSubsystems sender;
    Topics receiver;
    MessageContents contentDescription;
    std::string content;

    VSMMessage():contentDescription(Test),content(""),sender(S1),receiver(BROADCAST){}
    VSMMessage(VSMSubsystems sender, Topics receiver,    MessageContents contentDescription,std::string content):contentDescription(contentDescription),content(content),sender(sender),receiver(receiver){}

    VSMMessage(VSMMessage* old){
        this->contentDescription = old->contentDescription;
        this->content = old->content;
        this->receiver=old->receiver;
        this->sender = old->sender;
    }

    std::string toString(){
        return to_string((int)sender)+","+to_string((int)receiver)+","+to_string(contentDescription)+","+content;
        //VSMMessage()
    }
    static int stringToVsmMessage(std::string msgStr, VSMMessage *destMsg){
        vector<std::string> msgFields = split(msgStr);
        if(msgFields.size()!=4) return 0;
        try{
            destMsg->sender =static_cast<VSMSubsystems>(stoi(msgFields[0]));
            destMsg->receiver =static_cast<Topics>(stoi(msgFields[1]));
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
