#include "beaconTwoBehaviour.hpp"
#include "beaconOneBehaviour.hpp"

#include "twoPointFormationProtocol.hpp"

BeaconTwoBehaviour::BeaconTwoBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    beaconState = BeaconOneStates::TPFP;
    twoPointFormationProtocol = new TwoPointFormationProtocol(RoleInProtocol::MOVING_BEACON,this);
    twoPointFormationProtocol->start();
}


void BeaconTwoBehaviour::behaviourStep()
{
  switch(beaconState){
  case BeaconOneStates::TPFP:  // if protocol has ended check result and proceed with next protocol if result is ok
    if(twoPointFormationProtocol->tick()){
    if(twoPointFormationProtocol->wasSuccessful){
       // delete twoPointFormationProtocol;
        // continue with next protocol
    }
    }

      break;
  }
}
