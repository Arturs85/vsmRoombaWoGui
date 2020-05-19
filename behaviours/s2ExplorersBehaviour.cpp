#include "s2ExplorersBehaviour.hpp"
#include "explorerManagementProtocol.hpp"
#include "operationsManagementProtocol.hpp"

S2ExplorersBehaviour::S2ExplorersBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;
    s2type = S2Types::EXPLORERS;
type = VSMSubsystems::S2_EXPLORERS;
    explorerManagementProtocol = new ExplorerManagementProtocol(RoleInProtocol::S2EXPLORERS,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S2EXPLORERS,this);
    operationsManagementProtocol->start();

}


void S2ExplorersBehaviour::behaviourStep()
{
    S2BaseBehavior::behaviourStep();
    explorerManagementProtocol->tick();
    operationsManagementProtocol->tick();
}
