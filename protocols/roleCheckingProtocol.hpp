#ifndef ROLECHECKINGPROTOCOL_HPP
#define ROLECHECKINGPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class RoleCheckingProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
RoleCheckingProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


int retrysSoFar =0;
int intervalCounter =0;
virtual void start();
virtual bool tick();


private:

bool initiatorTick();
bool responderTick();

};





#endif //ROLECHECKINGPROTOCOL_HPP
