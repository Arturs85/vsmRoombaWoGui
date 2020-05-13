#include "s3Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "controlValueProtocol.hpp"
#include "operationsManagementProtocol.hpp"
#include "roombaAgent.hpp"


const std::vector<VSMSubsystems> S3Behaviour::requiredRoles ({VSMSubsystems::S2_BEACONS});//,VSMSubsystems::S2_EXPLORERS});

S3Behaviour::S3Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::RESPONDER,this);
    roleCheckingProtocol->start();
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::SENDER,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S3,this);
    operationsManagementProtocol->start();
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



int S3Behaviour::getActiveRobotsCount()// for initial control value calc
{
   int count =0;
std::map<int, double>::iterator it = knownAgents.begin();
double timeNow = owner->getSystemTimeSec();
while (it != knownAgents.end())
   {
   // Accessing VALUE from element pointed by it.
   double timeSeen = it->second;
if(timeNow-timeSeen<MARK_ABSENT_TIMEOUT)
count++;

   it++;
   }

  return count;
}

void S3Behaviour::behaviourStep()
{
    roleCheckingProtocol->tick();
    controlValueProtocol->tick();
operationsManagementProtocol->tick();
    switch (state) {

    }
}
