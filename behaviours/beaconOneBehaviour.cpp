#include "beaconOneBehaviour.hpp"
#include "twoPointFormationProtocol.hpp"
#include "thirdBeaconFormationProtocol.hpp"

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
                beaconState = BeaconOneStates::PFP;
                delete twoPointFormationProtocol;
                thirdBeaconFormationProtocol = new ThirdBeaconFormationProtocol(RoleInProtocol::STANDING_BEACON,this);
                // delete twoPointFormationProtocol;
                // continue with next protocol
            }
        }
        break;
    case BeaconOneStates::PFP:{
        if(thirdBeaconFormationProtocol->tick()){// end this protocol properly - switch to beacons operation
        }
    }
        break;
    }
}
