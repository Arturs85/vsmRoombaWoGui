#ifndef BEACONMASTERBEHAVIOUR_HPP
#define BEACONMASTERBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "beaconOneBehaviour.hpp"

class ThirdBeaconFormationProtocol;
class LocalisationProtocol;

class BeaconMasterBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    BeaconOneStates beaconState;
    RoombaAgent* ra;

    ThirdBeaconFormationProtocol* thirdBeaconFormationProtocol;
    LocalisationProtocol* localisationProtocol;

    BeaconMasterBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();
    virtual void remove();


};






#endif //BEACONMASTERBEHAVIOUR_HPP
