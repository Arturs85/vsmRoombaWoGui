#ifndef S3BEHAVIOUR_HPP
#define S3BEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
#include <map>
class RoleCheckingProtocol;
class ControlValueProtocol;
class OperationsManagementProtocol;

enum class S3States{BEACONS_FORMATION, EXPLORING, BEACONS_REFORMATION};
class S3Behaviour : public BaseCommunicationBehaviour {
public:
    S3Behaviour(RoombaAgent* owner);
    VSMSubsystems getUnfilledRole();// to get role when agent asks for it, returns NONE, if all roles are filled
void markAsFilled(VSMSubsystems role, int agentId);

private:
S3States state;
    RoleCheckingProtocol* roleCheckingProtocol;
    ControlValueProtocol* controlValueProtocol;
    OperationsManagementProtocol* operationsManagementProtocol;
    //std::map< int,int  > knownAgents;//<id,ms since last comm>, this map lets know, present agents and time since they was last heard from
std::map< VSMSubsystems,int  > behaviourCarieers;//<managementBehaviour, agenId>, this map shows, witch behaviours are filled and by whom

static const std::vector<VSMSubsystems> requiredRoles;//contains roles that are needed to be filled prior to start operations
std::vector<VSMSubsystems> unfilledRoles;// initialize it with requiredRoles;

void behaviourStep();

};

#endif //S3BEHAVIOUR_HPP
