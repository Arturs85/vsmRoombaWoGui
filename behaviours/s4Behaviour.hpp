#ifndef S4BEHAVIOUR_HPP
#define S4BEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
#include "s2BaseBehaviour.hpp"
#include <map>

class RoleCheckingProtocol;
class ControlValueProtocol;
class OperationsManagementProtocol;

#define MARK_ABSENT_TIMEOUT 30.0// seconds


enum class S4States{BEACONS_FORMATION, EXPLORING, BEACONS_REFORMATION};

class S4Behaviour : public BaseCommunicationBehaviour {
public:


    S4Behaviour(RoombaAgent* owner);
   

private:
S4States state;
  vector<PointF> mapPoints;
    OperationsManagementProtocol* operationsManagementProtocol;


void behaviourStep();

};

#endif //S4BEHAVIOUR_HPP
