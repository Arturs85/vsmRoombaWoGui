#ifndef BEACONMASTERBEHAVIOUR_HPP
#define BEACONMASTERBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"

class ThirdBeaconFormationProtocol;


class BeaconMasterBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    ThirdBeaconFormationProtocol* thirdBeaconFormationProtocol;

    BeaconMasterBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();
    virtual void remove();


};






#endif //BEACONMASTERBEHAVIOUR_HPP
