#ifndef S1BEHAVIOUR_HPP
#define S1BEHAVIOUR_HPP
#include "roombaAgent.hpp"

class BeaconManagementProtocol;


class S1Behaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
 public:

    RoombaAgent* ra;
   

    S1Behaviour(RoombaAgent *roombaAgent);
private:
    void behaviourStep();
BeaconManagementProtocol* beaconManagementProtocol;

};






#endif //S1BEHAVIOUR_HPP
