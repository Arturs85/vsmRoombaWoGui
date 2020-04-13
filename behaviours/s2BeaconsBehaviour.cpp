#include "s2BeaconsBehaviour.hpp"
#include "beaconManagementProtocol.hpp"


S2BeaconsBehaviour::S2BeaconsBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;

    beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::S2BEACON,this);
}


void S2BeaconsBehaviour::behaviourStep()
{
    beaconManagementProtocol->tick();
}
