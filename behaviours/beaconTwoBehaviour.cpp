#include "beaconTwoBehaviour.hpp"
#include "beaconOneBehaviour.hpp"

#include "twoPointFormationProtocol.hpp"
#include "thirdBeaconFormationProtocol.hpp"

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
                beaconState = BeaconOneStates::PFP;
                delete twoPointFormationProtocol;
                thirdBeaconFormationProtocol = new ThirdBeaconFormationProtocol(RoleInProtocol::STANDING_BEACON,this);
                thirdBeaconFormationProtocol->start();// use start only in standing role

                std::cout<<"beacon two changing to 3rd bfp (pfp)\n";
            }
        }

        break;
    case BeaconOneStates::PFP:{
        if(thirdBeaconFormationProtocol->tick()){// end this protocol properly - switchh to beacons operation
        }
    }
        break;
    }
}

void BeaconTwoBehaviour::remove()
{
    BaseCommunicationBehaviour::remove();
    ra->uwbMsgListener.idFromBeaconType=0;//erease id so uwb listener does not respond to triang measurements

}
