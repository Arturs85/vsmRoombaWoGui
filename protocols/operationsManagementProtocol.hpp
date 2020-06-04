#ifndef OPERATIONSMANAGEMENTPROTOCOL_HPP
#define OPERATIONSMANAGEMENTPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

#define REPLY_WAITING_TICKS 5/TICK_PERIOD_SEC
#define BEACONS_COUNT_NORMAL 3
#define BEACONS_COUNT_REFORMATION 6

class OperationsManagementProtocol: public BaseProtocol {// protocol between s3 and s2, manages order and timing of actions
public:
OperationsManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

virtual void start();
virtual bool tick();

void initiateBeaconsRegroup();

private:

bool s3Tick();
bool s2BeaconsTick();
bool s2ExplorersTick();

void enterState(ProtocolStates stateToEnter);

bool s4Tick();



};





#endif //OPERATIONSMANAGEMENTPROTOCOL_HPP
