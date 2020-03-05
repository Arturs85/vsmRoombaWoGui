#include "roleCheckingBehaviour.hpp"


RoleCheckingBehaviour::RoleCheckingBehaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::INITIATOR,this);
    roleCheckingProtocol->start();
}

void RoleCheckingBehaviour::behaviourStep()
{
roleCheckingProtocol->tick();
}

void RoleCheckingBehaviour::askS3ForRoles()
{

}
