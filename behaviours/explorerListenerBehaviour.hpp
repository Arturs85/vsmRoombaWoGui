#ifndef EXPLORERLISTENERBEHAVIOUR_HPP
#define EXPLORERLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

//class BeaconManagementProtocol;

enum class ExplorerStates{IDLE,EXPLORING,MEASURING_XY};

class LocalisationProtocol;

class ExplorerListenerBehaviour: public BaseCommunicationBehaviour{// listen s2 commands and explore space, localising time after time
 public:

    RoombaAgent* ra;
  LocalisationProtocol* localisationProtocol;
void localise();
    ExplorerListenerBehaviour(RoombaAgent *roombaAgent);
    void startExploring();//to call after comand from s2 is received
private:
    void behaviourStep();
ExplorerStates explorerState = ExplorerStates::IDLE;
//BeaconManagementProtocol* beaconManagementProtocol;

};






#endif //EXPLORERLISTENERBEHAVIOUR_HPP
