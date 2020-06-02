#ifndef EXPLORERLISTENERBEHAVIOUR_HPP
#define EXPLORERLISTENERBEHAVIOUR_HPP
#include "roombaAgent.hpp"

class ExplorerManagementProtocol;

enum class ExplorerStates{IDLE,EXPLORING,MEASURING_XY, MOVING_FORWARD,TURNING,ARRIVED_DEST,ENCOUNTERED_OBST,ARRIVED_DEST_AND_LOCALISED,FIRST_MEASUREMENT, FIRST_DRIVE, FINAL_DESTINATION};
struct PointInt{int x;int y;};

class LocalisationProtocol;

class ExplorerListenerBehaviour: public BaseCommunicationBehaviour{// listen s2 commands and explore space, localising time after time
 public:

    RoombaAgent* ra;
  LocalisationProtocol* localisationProtocol;
void localise(ExplorerStates nextState);

    ExplorerListenerBehaviour(RoombaAgent *roombaAgent);
    void startExploring();//to call after comand from s2 is received
    void stopExploring();//to call after comand from s2 is received
PointInt previousLocation;
    PointInt latestLocation;
float latestDirection;
int distToTravelAfterTurning =0;

static PointInt getRandomPointAtDistance(int distance);
private:
    void behaviourStep();
ExplorerStates explorerState = ExplorerStates::IDLE;
ExplorerStates stateAfterLocalise = ExplorerStates::IDLE;

ExplorerManagementProtocol* explorerManagementProtocol;
int odometryBeforeDriving=0;


};






#endif //EXPLORERLISTENERBEHAVIOUR_HPP
