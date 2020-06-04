#include "s2BaseBehaviour.hpp"
#include "controlValueProtocol.hpp"
#include "s1ExchangeProtocol.hpp"
#include "roleCheckingProtocol.hpp"

S2BaseBehavior::S2BaseBehavior(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::RECEIVER,this);
    s1ExchangeProtocol = new S1ExchangeProtocol(this);
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::S2BASE,this);
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

}
