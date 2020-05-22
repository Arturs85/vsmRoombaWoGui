#include "beaconTwoBehaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "beaconMasterBehaviour.hpp"
#include "twoPointFormationProtocol.hpp"
#include "thirdBeaconFormationProtocol.hpp"
#include "localisationProtocol.hpp"


BeaconMasterBehaviour::BeaconMasterBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    beaconState = BeaconOneStates::PFP;
    thirdBeaconFormationProtocol = new ThirdBeaconFormationProtocol(RoleInProtocol::MOVING_BEACON,this);
  //  twoPointFormationProtocol->start();
    localisationProtocol = new LocalisationProtocol(RoleInProtocol::BEACON_MASTER,this);
}


void BeaconMasterBehaviour::behaviourStep()
{
  switch(beaconState){
  case BeaconOneStates::PFP:  // if protocol has ended check result and proceed with next protocol if result is ok
    if(thirdBeaconFormationProtocol->tick()){
    if(thirdBeaconFormationProtocol->wasSuccessful){

        // delete twoPointFormationProtocol;
        // continue with next protocol
    beaconState= BeaconOneStates::BEACON;
    std::cout<<" bmb entering beacon state\n";
    }
    }

      break;
 case BeaconOneStates::BEACON:{
  //act as beacon for triangulation
      localisationProtocol->tick();
  }
  break;
  }
}

void BeaconMasterBehaviour::remove()
{
    BaseCommunicationBehaviour::remove();
    ra->uwbMsgListener.idFromBeaconType=0;//erease id so uwb listener does not respond to triang measurements
}
