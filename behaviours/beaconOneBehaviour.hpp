#ifndef BEACONONEBEHAVIOUR_HPP
#define BEACONONEBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class TwoPointFormationProtocol;

enum class BeaconOneStates{IDLE,FINISHED,TPFP,PFP,BEACON};

class BeaconOneBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
    BeaconOneStates beaconState;
    RoombaAgent* ra;
    TwoPointFormationProtocol* twoPointFormationProtocol;
    BeaconOneBehaviour(RoombaAgent* roombaAgent);
    void behaviourStep();


};






#endif //BEACONONEBEHAVIOUR_HPP
