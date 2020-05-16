#include "explorerListenerBehaviour.hpp"
#include "beaconManagementProtocol.hpp"

// add this behaviour when unit becomes of explore type, so it can listen to commands from s2
ExplorerListenerBehaviour::ExplorerListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
//beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::BEACON,this);
}


void ExplorerListenerBehaviour::behaviourStep()
{
// beaconManagementProtocol->tick();
//manage driving



}

