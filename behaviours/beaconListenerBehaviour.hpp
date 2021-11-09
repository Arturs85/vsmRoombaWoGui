#ifndef BEACONLISTENERBEHAVIOUR_HPP
#define BEACONLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class BeaconManagementProtocolStatic;
class LocalisationProtocolPf;

class BeaconListenerBehaviour: public BaseCommunicationBehaviour{// waits for requests from second beacon by TwoPoinFormation protocol
 public:

    RoombaAgent* ra;
   

    BeaconListenerBehaviour(RoombaAgent *roombaAgent);
private:
    void behaviourStep();
BeaconManagementProtocolStatic* beaconManagementProtocol;
LocalisationProtocolPf* localisationProtocol;
};






#endif //BEACONLISTENERBEHAVIOUR_HPP
