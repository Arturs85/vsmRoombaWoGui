#include "s3Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "controlValueProtocol.hpp"
#include "operationsManagementProtocol.hpp"
#include "roombaAgent.hpp"


const std::vector<VSMSubsystems> S3Behaviour::requiredRoles ({VSMSubsystems::S2_BEACONS,VSMSubsystems::S2_EXPLORERS,VSMSubsystems::S4});//,VSMSubsystems::S2_EXPLORERS});

S3Behaviour::S3Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    unfilledRoles = requiredRoles;// copy contents of vector
    type=VSMSubsystems::S3;
    cvals=vector<int>((int)S2Types::SIZE_OF_THIS_ENUM,0);//initialize cvals according to the size of defined s2 role count
    std::cout<<"s3 -sizeOf cvals at constructor  "<<cvals.size()<<"\n";
    knownAgents.emplace(owner->id,owner->getSystemTimeSec());//mark itself as available robot for cvalue calculation

    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::RESPONDER,this);
    roleCheckingProtocol->start();
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::SENDER,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S3,this);
    operationsManagementProtocol->start();
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
    knownAgents.at(owner->id)= owner->getSystemTimeSec();//update self availability

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

void S3Behaviour::updateCvals(int beaconsNeeded)
{
    int robotsAlvail = getActiveRobotsCount();
    //std::cout<<"s3 -robots avail "<<robotsAlvail<<"\n";
    //std::cout<<"s3 -sizeOf cvals  "<<cvals.size()<<"\n";

   // cvals.at(S2Types::BEACONS)=beaconsNeeded;
   // cvals.at(S2Types::EXPLORERS)=robotsAlvail-beaconsNeeded;
    cvals.at(S2Types::BEACONS)=robotsAlvail-1; // for haveing at least one explorer
    cvals.at(S2Types::EXPLORERS)=1;
}

void S3Behaviour::behaviourStep()
{
    roleCheckingProtocol->tick();
    controlValueProtocol->tick();
    operationsManagementProtocol->tick();
    //   switch (state) {

    // }
}
