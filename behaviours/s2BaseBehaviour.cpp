#include "s2BaseBehaviour.hpp"
#include "controlValueProtocol.hpp"
#include "s1ExchangeProtocol.hpp"
#include "roleCheckingProtocol.hpp"
#include "roombaAgent.hpp"
#include "baseS1ManagementProtocol.hpp"
#include <map>
#define MARK_ABSENT_TIMEOUT_S2 30

S2BaseBehavior::S2BaseBehavior(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::RECEIVER,this);
    s1ExchangeProtocol = new S1ExchangeProtocol(this);
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::S2BASE,this);

    if(owner->s1Type==type)
        knownS1OfType.emplace(owner->id,owner->getSystemTimeSec());//add itself to the time table, if type is coresp.

}

S2BaseBehavior::~S2BaseBehavior()
{

}

void S2BaseBehavior::resetRoleCheckingProtocol()
{
    if(roleCheckingProtocol!=0){
        delete roleCheckingProtocol;
        roleCheckingProtocol=0;
    }
    rcpAutoAddCounter = rcpAutoAddAfterTicks;

}

int S2BaseBehavior::getS1IdForGiveaway()
{
    return 0;
}

void S2BaseBehavior::ereaseMissingRobots()
{
    if(owner->s1Type==type)
        knownS1OfType.at(owner->id)= owner->getSystemTimeSec();//update self availability

    std::map<int, double>::iterator it = knownS1OfType.begin();
    double timeNow = owner->getSystemTimeSec();
    while (it != knownS1OfType.end())
    {
        // Accessing VALUE from element pointed by it.
        double timeSeen = it->second;
        if(timeNow-timeSeen>MARK_ABSENT_TIMEOUT_S2)
            //erease id from available and used list
            s1ManagementProtocol->availableRobotsSet.erase(it->first);
        s1ManagementProtocol->usedRobots.erase(it->first);
        std::cout<<"s2baseBeh unregistering "<< it->first<<"\n";
        it++;
    }
}

void S2BaseBehavior::behaviourStep()
{
    controlValueProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("cvp s2 inherited step returned \n");

    s1ExchangeProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("s1e s2 inherited step returned \n");

    BaseCommunicationBehaviour::logKeypoints("rcp s2 inherited step returned \n");

    if(roleCheckingProtocol == 0){
        rcpAutoAddCounter--;
        if(rcpAutoAddCounter<=0){
            //readd  rcp
            roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::S2BASE,this);
        }
    }else
        roleCheckingProtocol->tick();

    ereaseMissingRobots();
}
