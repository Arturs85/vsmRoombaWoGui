#ifndef ROLECHECKINGPROTOCOL_HPP
#define ROLECHECKINGPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class RoleCheckingProtocol: public BaseProtocol {

RoleCheckingProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

int retrysSoFar =0;
   virtual bool tick();
virtual void start();
bool initiatorTick();
bool responderTick();

};





#endif //ROLECHECKINGPROTOCOL_HPP
