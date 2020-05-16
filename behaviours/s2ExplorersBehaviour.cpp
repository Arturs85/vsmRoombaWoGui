#include "s2ExplorersBehaviour.hpp"
#include "beaconManagementProtocol.hpp"
#include "operationsManagementProtocol.hpp"

S2ExplorersBehaviour::S2ExplorersBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;
s2type = S2Types::EXPLORERS;
    //beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::S2BEACON,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S2BEACON,this);
operationsManagementProtocol->start();
}


void S2ExplorersBehaviour::behaviourStep()
{
   S2BaseBehavior::behaviourStep();
    //beaconManagementProtocol->tick();
    operationsManagementProtocol->tick();
}
