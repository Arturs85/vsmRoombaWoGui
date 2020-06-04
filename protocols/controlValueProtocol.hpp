#ifndef CONTROLVALUEPROTOCOL_HPP
#define CONTROLVALUEPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class ControlValueProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
ControlValueProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

vector<int> cVals;
virtual void start();
virtual bool tick();

void sendCvals(vector<int> cVals);
int beaconsCountForCurrentState =3;//to be changed by s3
private:
int tickCount=0;
bool senderTick();
bool receiverTick();


};





#endif //CONTROLVALUEPROTOCOL_HPP
