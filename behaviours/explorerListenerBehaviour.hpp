#ifndef EXPLORERLISTENERBEHAVIOUR_HPP
#define EXPLORERLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class BeaconManagementProtocol;


class ExplorerListenerBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
 public:

    RoombaAgent* ra;
   

    ExplorerListenerBehaviour(RoombaAgent *roombaAgent);
private:
    void behaviourStep();

BeaconManagementProtocol* beaconManagementProtocol;

};






#endif //EXPLORERLISTENERBEHAVIOUR_HPP
