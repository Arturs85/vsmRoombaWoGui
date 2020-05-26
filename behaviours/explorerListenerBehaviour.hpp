#ifndef EXPLORERLISTENERBEHAVIOUR_HPP
#define EXPLORERLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class ExplorerManagementProtocol;

enum class ExplorerStates{IDLE,EXPLORING,MEASURING_XY};
struct PointInt{int x;int y;};

class LocalisationProtocol;

class ExplorerListenerBehaviour: public BaseCommunicationBehaviour{// listen s2 commands and explore space, localising time after time
 public:

    RoombaAgent* ra;
  LocalisationProtocol* localisationProtocol;
void localise();
    ExplorerListenerBehaviour(RoombaAgent *roombaAgent);
    void startExploring();//to call after comand from s2 is received
    void stopExploring();//to call after comand from s2 is received
static PointInt getRandomPointAtDistance(int distance);
private:
    void behaviourStep();
ExplorerStates explorerState = ExplorerStates::IDLE;
ExplorerManagementProtocol* explorerManagementProtocol;

};






#endif //EXPLORERLISTENERBEHAVIOUR_HPP
