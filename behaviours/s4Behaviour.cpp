#include "s4Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "localisationProtocol.hpp"
#include "operationsManagementProtocol.hpp"
#include "roombaAgent.hpp"



S4Behaviour::S4Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    type=VSMSubsystems::S4;
     

   
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S4,this);
    operationsManagementProtocol->start();

    localisationProtocol = new LocalisationProtocol(RoleInProtocol::S4,this);
}

int S4Behaviour::addPointToMap(int x, int y)
{
PointF p = {x,y};
mapPoints.push_back(p);
return mapPoints.size();
}

void S4Behaviour::InitiateBeaconsRegroup()
{
   operationsManagementProtocol->initiateBeaconsRegroup();
}




void S4Behaviour::behaviourStep()
{
    operationsManagementProtocol->tick();
 localisationProtocol->tick();// listens to the coordinate points of explorers
}
