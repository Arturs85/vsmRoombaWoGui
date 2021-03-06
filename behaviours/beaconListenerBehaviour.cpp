#include "beaconListenerBehaviour.hpp"
#include "beaconManagementProtocol.hpp"

// add this behaviour when unit becomes of beacon type, so it can listen to commands from s2, commands are which particular beacon behaviour to take
BeaconListenerBehaviour::BeaconListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::BEACON,this);
type = VSMSubsystems::S1_BEACONS;
}


void BeaconListenerBehaviour::behaviourStep()
{
 beaconManagementProtocol->tick();
             //cout<<"bmp beacons tick returned \n";

}
