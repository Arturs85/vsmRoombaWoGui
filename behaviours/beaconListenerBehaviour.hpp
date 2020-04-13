#ifndef BEACONLISTENERBEHAVIOUR_HPP
#define BEACONLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class BeaconManagementProtocol;


class BeaconListenerBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
 public:

    RoombaAgent* ra;
   

    BeaconListenerBehaviour(RoombaAgent *roombaAgent);
private:
    void behaviourStep();
BeaconManagementProtocol* beaconManagementProtocol;

};






#endif //BEACONLISTENERBEHAVIOUR_HPP
