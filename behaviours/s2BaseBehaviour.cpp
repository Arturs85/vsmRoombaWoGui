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

    roleCheckingProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("rcp s2 inherited step returned \n");

}
