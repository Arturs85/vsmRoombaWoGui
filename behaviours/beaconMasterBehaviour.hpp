#ifndef BEACONMASTERBEHAVIOUR_HPP
#define BEACONMASTERBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"

class ThirdBeaconFormationProtocol;


class BeaconMasterBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    ThirdBeaconFormationProtocol* thirdBeaconFormationProtocol;
    BeaconMasterBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();


};






#endif //BEACONMASTERBEHAVIOUR_HPP
