#ifndef MOVETOTARGETBEHAVIOUR_HPP
#define MOVETOTARGETBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "explorerListenerBehaviour.hpp"

class BeaconsManagementProtocol;

//enum class ExplorerStates{IDLE,EXPLORING,MEASURING_XY, MOVING_FORWARD,TURNING,ARRIVED_DEST,ENCOUNTERED_OBST,ARRIVED_DEST_AND_LOCALISED,FIRST_MEASUREMENT, FIRST_DRIVE};


class LocalisationProtocol;

class MoveToTargetBehaviour: public BaseCommunicationBehaviour{// 
public:

    RoombaAgent* ra;
    LocalisationProtocol* localisationProtocol;
    void localise(ExplorerStates nextState);
vector<PointInt> route;
    MoveToTargetBehaviour(RoombaAgent *roombaAgent);

    PointInt previousLocation;
    PointInt latestLocation;
    float latestDirection;
    int distToTravelAfterTurning =0;

    PointInt getNextPointToTravel();
    void startRoute();

private:
    PointInt destinationPoint;
    void behaviourStep();
    ExplorerStates movementState = ExplorerStates::IDLE;
    ExplorerStates stateAfterLocalise;
    int odometryBeforeDriving=0;


};






#endif //MOVETOTARGETBEHAVIOUR_HPP
