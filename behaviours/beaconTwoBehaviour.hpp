#ifndef BEACONTWOBEHAVIOUR_HPP
#define BEACONTWOBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"

class TwoPointFormationProtocol;

class BeaconTwoBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    TwoPointFormationProtocol* twoPointFormationProtocol;
    BeaconTwoBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();


};






#endif //BEACONTWOBEHAVIOUR_HPP
