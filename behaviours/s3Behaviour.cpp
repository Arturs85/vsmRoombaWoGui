#include "s3Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "controlValueProtocol.hpp"

const std::vector<VSMSubsystems> S3Behaviour::requiredRoles ({VSMSubsystems::S2_BEACONS,VSMSubsystems::S2_EXPLORERS});

S3Behaviour::S3Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::RESPONDER,this);
    roleCheckingProtocol->start();
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::SENDER,this);
    unfilledRoles = requiredRoles;// copy contents of vector
    type=VSMSubsystems::S3;
}

VSMSubsystems S3Behaviour::getUnfilledRole()
{
    if(unfilledRoles.empty())return VSMSubsystems::NONE;
    return unfilledRoles.at(0); // just get first unfilled role
}

void S3Behaviour::markAsFilled(VSMSubsystems role, int agentId)
{
    //mark owner of role
    auto search = behaviourCarieers.find(role);

    if (search != behaviourCarieers.end()) {
        behaviourCarieers.at(role)=agentId;
    } else {
        behaviourCarieers.emplace(role,agentId);

    }


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
    controlValueProtocol->tick();

    switch (state) {

    }
}
