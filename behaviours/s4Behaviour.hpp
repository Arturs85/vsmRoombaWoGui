#ifndef S4BEHAVIOUR_HPP
#define S4BEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
#include "s2BaseBehaviour.hpp"
#include <map>
#include "explorerListenerBehaviour.hpp"


class RoleCheckingProtocol;
class LocalisationProtocol;
class OperationsManagementProtocol;

#define MARK_ABSENT_TIMEOUT 30.0// seconds


enum class S4States{BEACONS_FORMATION, EXPLORING, BEACONS_REFORMATION};

class S4Behaviour : public BaseCommunicationBehaviour {
public:


    S4Behaviour(RoombaAgent* owner);
   
int addPointToMap(int x, int y);
void InitiateBeaconsRegroup();//to use when current area is explored
vector<int> getCordsForRegrouping(int side);
PointInt getFaresPoint();

private:
S4States state;
  vector<PointInt> mapPoints;
    OperationsManagementProtocol* operationsManagementProtocol;
    LocalisationProtocol* localisationProtocol;


void behaviourStep();

};

#endif //S4BEHAVIOUR_HPP
