#ifndef OPERATIONSMANAGEMENTPROTOCOL_HPP
#define OPERATIONSMANAGEMENTPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class OperationsManagementProtocol: public BaseProtocol {// protocol between s3 and s2, manages order and timing of actions
public:
OperationsManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

virtual void start();
virtual bool tick();


private:

bool s3Tick();
bool s2Tick();
void enterState(ProtocolStates stateToEnter);

};





#endif //OPERATIONSMANAGEMENTPROTOCOL_HPP
