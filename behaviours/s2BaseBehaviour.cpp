#include "s2BaseBehaviour.hpp"
#include "controlValueProtocol.hpp"
#include "s1ExchangeProtocol.hpp"

S2BaseBehavior::S2BaseBehavior(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::RECEIVER,this);
s1ExchangeProtocol = new S1ExchangeProtocol(this);
}

void S2BaseBehavior::behaviourStep()
{
    controlValueProtocol->tick();
    s1ExchangeProtocol->tick();
}
S2BaseBehavior::~S2BaseBehavior()
{
    
}

int S2BaseBehavior::getS1IdForGiveaway()
{
    
}
