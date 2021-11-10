#include "beaconListenerBehaviour.hpp"
#include "beaconManagementProtocolStatic.hpp"
#include "localisationProtocolpf.hpp"
// add this behaviour when unit becomes of beacon type, so it can listen to commands from s2, commands are which particular beacon behaviour to take
BeaconListenerBehaviour::BeaconListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
beaconManagementProtocol = new BeaconManagementProtocolStatic(RoleInProtocol::BEACON,this);
type = VSMSubsystems::S1_BEACONS;

//added localisation protocol here so that all beacon listeners accept measurement requests
  localisationProtocol=new LocalisationProtocolPf(RoleInProtocol::RESPONDER,this);
}


void BeaconListenerBehaviour::behaviourStep()
{
 beaconManagementProtocol->tick();
             //cout<<"bmp beacons tick returned \n";
localisationProtocol->tick();
}
