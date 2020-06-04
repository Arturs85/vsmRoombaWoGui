#include "s4Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "localisationProtocol.hpp"
#include "operationsManagementProtocol.hpp"
#include "roombaAgent.hpp"
#include <cmath>


S4Behaviour::S4Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    type=VSMSubsystems::S4;



    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S4,this);
    operationsManagementProtocol->start();

    localisationProtocol = new LocalisationProtocol(RoleInProtocol::S4,this);
}

int S4Behaviour::addPointToMap(int x, int y)
{
    PointInt p = {x,y};
    mapPoints.push_back(p);
    return mapPoints.size();
}

void S4Behaviour::InitiateBeaconsRegroup()
{
    operationsManagementProtocol->initiateBeaconsRegroup();
}

vector<int> S4Behaviour::getCordsForRegrouping(int side)
{
    PointInt destPoint = getFaresPoint();

    float angle = atan2(destPoint.y,destPoint.x);
    float a1 = angle -PI*30/180;
    float a2 = angle +PI*30/180;
    int x1 = side*cos(a1);
    int y1 = side*sin(a1);
    int x2 = side*cos(a2);
    int y2 = side*sin(a2);
    return vector<int>{destPoint.x,destPoint.y,destPoint.x+x1,destPoint.y+y1,destPoint.x+x2,destPoint.y+y2};

}

PointInt S4Behaviour::getFaresPoint()
{
    float distSoFarSqred =0;
    PointInt result;
    for (int i = 0; i < mapPoints.size(); ++i) {
        PointInt p= mapPoints.at(i);
        if(distSoFarSqred< p.x*p.x+p.y*p.y){
            distSoFarSqred =p.x*p.x+p.y*p.y;
            result = p;
        }

    }
    return result;
}




void S4Behaviour::behaviourStep()
{
    operationsManagementProtocol->tick();
    localisationProtocol->tick();// listens to the coordinate points of explorers
}
