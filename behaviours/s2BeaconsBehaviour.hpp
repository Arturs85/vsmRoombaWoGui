#ifndef S2BEACONSBEHAVIOUR_HPP
#define S2BEACONSBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "s2BaseBehaviour.hpp"

class BeaconManagementProtocol;

class S2BeaconsBehaviour: public S2BaseBehavior{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    S2BeaconsBehaviour(RoombaAgent* roombaAgent);
private:
    void behaviourStep();
    BeaconManagementProtocol* beaconManagementProtocol;


};






#endif //S2BEACONSBEHAVIOUR_HPP
