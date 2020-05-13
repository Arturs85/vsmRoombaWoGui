#ifndef S3BEHAVIOUR_HPP
#define S3BEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
#include "s2BaseBehaviour.hpp"
#include <map>

class RoleCheckingProtocol;
class ControlValueProtocol;
class OperationsManagementProtocol;

#define MARK_ABSENT_TIMEOUT 30.0// seconds


enum class S3States{BEACONS_FORMATION, EXPLORING, BEACONS_REFORMATION};

class S3Behaviour : public BaseCommunicationBehaviour {
public:


    S3Behaviour(RoombaAgent* owner);
    VSMSubsystems getUnfilledRole();// to get role when agent asks for it, returns NONE, if all roles are filled

    void markAsFilled(VSMSubsystems role, int agentId);
    std::map< int,double  > knownAgents;//<id,abs time when last comm >, this map lets know, present agents and time since they was last heard from
int getActiveRobotsCount();//return nr of robots that has used rcp protocol recently
void updateCvals();
private:
vector<int> cvals;
S3States state;
    RoleCheckingProtocol* roleCheckingProtocol;
    ControlValueProtocol* controlValueProtocol;
    OperationsManagementProtocol* operationsManagementProtocol;
std::map< VSMSubsystems,int  > behaviourCarieers;//<managementBehaviour, agenId>, this map shows, witch behaviours are filled and by whom

static const std::vector<VSMSubsystems> requiredRoles;//contains roles that are needed to be filled prior to start operations
std::vector<VSMSubsystems> unfilledRoles;// initialize it with requiredRoles;

void behaviourStep();

};

#endif //S3BEHAVIOUR_HPP
