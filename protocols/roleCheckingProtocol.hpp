#ifndef ROLECHECKINGPROTOCOL_HPP
#define ROLECHECKINGPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class RoleCheckingProtocol: public BaseProtocol {
public:
RoleCheckingProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


int retrysSoFar =0;
virtual void start();
virtual bool tick();


private:

bool initiatorTick();
bool responderTick();

};





#endif //ROLECHECKINGPROTOCOL_HPP
