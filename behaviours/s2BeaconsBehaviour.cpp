#include "s2BeaconsBehaviour.hpp"
#include "beaconManagementProtocol.hpp"
#include "operationsManagementProtocol.hpp"

S2BeaconsBehaviour::S2BeaconsBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;

    beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::S2BEACON,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S2BEACON,this);
operationsManagementProtocol->start();
}


void S2BeaconsBehaviour::behaviourStep()
{
    S2BaseBehavior::behaviourStep();

    beaconManagementProtocol->tick();
    operationsManagementProtocol->tick();
}
