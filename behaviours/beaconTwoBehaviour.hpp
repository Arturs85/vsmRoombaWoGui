#ifndef BEACONTWOBEHAVIOUR_HPP
#define BEACONTWOBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"

class TwoPointFormationProtocol;
class ThirdBeaconFormationProtocol;

class BeaconTwoBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    TwoPointFormationProtocol* twoPointFormationProtocol;
    ThirdBeaconFormationProtocol* thirdBeaconFormationProtocol;

    BeaconTwoBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();
    virtual void remove();


};






#endif //BEACONTWOBEHAVIOUR_HPP
