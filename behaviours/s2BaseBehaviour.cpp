#include "s2BaseBehaviour.hpp"
#include "controlValueProtocol.hpp"


S2BaseBehavior::S2BaseBehavior(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    controlValueProtocol = new ControlValueProtocol(RoleInProtocol::RECEIVER,this);
}

void S2BaseBehavior::behaviourStep()
{
    controlValueProtocol->tick();
}
