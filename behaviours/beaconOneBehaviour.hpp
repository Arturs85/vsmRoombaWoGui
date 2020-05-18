#ifndef BEACONONEBEHAVIOUR_HPP
#define BEACONONEBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class TwoPointFormationProtocol;
class ThirdBeaconFormationProtocol;
enum class BeaconRoles{BEACON_ONE,BEACON_TWO,BEACON_MASTER};
enum class BeaconOneStates{IDLE,FINISHED,TPFP,PFP,BEACON};

class BeaconOneBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
 public:
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    TwoPointFormationProtocol* twoPointFormationProtocol;
   ThirdBeaconFormationProtocol* thirdBeaconFormationProtocol;

    BeaconOneBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();
    virtual void remove();


};






#endif //BEACONONEBEHAVIOUR_HPP
