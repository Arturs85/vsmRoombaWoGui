#ifndef S2BEACONSBEHAVIOUR_HPP
#define S2BEACONSBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "s2BaseBehaviour.hpp"


class S2BeaconsBehaviour: public S2BaseBehavior{// waits for requests from second beacon by TwoPoinFormation protocol
   
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    S2BeaconsBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();


};






#endif //S2BEACONSBEHAVIOUR_HPP
