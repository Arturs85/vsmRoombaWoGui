#include "beaconOneBehaviour.hpp"
#include "twoPointFormationProtocol.hpp"

BeaconOneBehaviour::BeaconOneBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    beaconState = BeaconOneStates::TPFP;
    twoPointFormationProtocol = new TwoPointFormationProtocol(RoleInProtocol::STANDING_BEACON,this);
    twoPointFormationProtocol->start();
}


void BeaconOneBehaviour::behaviourStep()
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
