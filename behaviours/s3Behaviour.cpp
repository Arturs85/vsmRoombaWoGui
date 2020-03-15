#include "s3Behaviour.hpp"
#include "roleCheckingProtocol.hpp"

const std::vector<VSMSubsystems> S3Behaviour::requiredRoles ({VSMSubsystems::S2_BEACONS,VSMSubsystems::S2_EXPLORERS});

S3Behaviour::S3Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::RESPONDER,this);
    roleCheckingProtocol->start();
}

VSMSubsystems S3Behaviour::getUnfilledRole()
{
    if(unfilledRoles.empty())return VSMSubsystems::NONE;
    return unfilledRoles.at(0); // just get first unfilled role
}

void S3Behaviour::markAsFilled(VSMSubsystems role, int agentId)
{
    //mark owner of role
    behaviourCarieers.insert_or_assign(role,agentId);
    //    auto search = behaviourCarieers.find(role);

    //    if (search != behaviourCarieers.end()) {

    //    } else {
    //       }


    // remove role from unfilledRoles list
    vector<VSMSubsystems >::iterator it = unfilledRoles.begin();

    while(it != unfilledRoles.end()) {
        if(*it==role)
        {
            it = unfilledRoles.erase(it);
            return;
        }
        else ++it;
    }
}

void S3Behaviour::behaviourStep()
{
    roleCheckingProtocol->tick();
}
