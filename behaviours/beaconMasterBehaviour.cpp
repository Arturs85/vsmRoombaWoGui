#include "beaconTwoBehaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "beaconMasterBehaviour.hpp"
#include "twoPointFormationProtocol.hpp"
#include "thirdBeaconFormationProtocol.hpp"



BeaconMasterBehaviour::BeaconMasterBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    beaconState = BeaconOneStates::PFP;
    thirdBeaconFormationProtocol = new ThirdBeaconFormationProtocol(RoleInProtocol::MOVING_BEACON,this);

  //  twoPointFormationProtocol->start();
}


void BeaconMasterBehaviour::behaviourStep()
{
  switch(beaconState){
  case BeaconOneStates::PFP:  // if protocol has ended check result and proceed with next protocol if result is ok
    if(thirdBeaconFormationProtocol->tick()){
   // if(twoPointFormationProtocol->wasSuccessful){
       // delete twoPointFormationProtocol;
        // continue with next protocol
    //}
    }

      break;
  }
}
