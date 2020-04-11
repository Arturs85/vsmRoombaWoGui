#ifndef BEACONMASTERBEHAVIOUR_HPP
#define BEACONMASTERBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"


class BeaconMasterBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
    BeaconOneStates beaconState;
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    BeaconMasterBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();


};






#endif //BEACONMASTERBEHAVIOUR_HPP
