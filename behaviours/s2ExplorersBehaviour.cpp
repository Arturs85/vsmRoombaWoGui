#include "s2ExplorersBehaviour.hpp"
#include "explorerManagementProtocol.hpp"
#include "operationsManagementProtocol.hpp"

S2ExplorersBehaviour::S2ExplorersBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;
    s2type = S2Types::EXPLORERS;
    type = VSMSubsystems::S2_EXPLORERS;
    s1ManagementProtocol = new ExplorerManagementProtocol(RoleInProtocol::S2EXPLORERS,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S2EXPLORERS,this);
    operationsManagementProtocol->start();

}

void S2ExplorersBehaviour::enterIdleState()
{
    ((ExplorerManagementProtocol*)s1ManagementProtocol)->enterIdleState();
}

void S2ExplorersBehaviour::enterExploringState()
{
    ((ExplorerManagementProtocol*)s1ManagementProtocol)->start();
}


void S2ExplorersBehaviour::behaviourStep()
{
    S2BaseBehavior::behaviourStep();
       
    ((ExplorerManagementProtocol*)s1ManagementProtocol)->tick();
      BaseCommunicationBehaviour::logKeypoints("emp s2 ex step returned \n");

    operationsManagementProtocol->tick();
            BaseCommunicationBehaviour::logKeypoints("omp s2 ex step returned \n");

}

int S2ExplorersBehaviour::getS1IdForGiveaway()
{
 return s1ManagementProtocol->getUnusedBeaconId();
}
