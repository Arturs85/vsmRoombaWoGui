#include "s3Behaviour.hpp"
#include "roleCheckingProtocol.hpp"

S3Behaviour::S3Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::RESPONDER,this);
    roleCheckingProtocol->start();
}

void S3Behaviour::behaviourStep()
{
    roleCheckingProtocol->tick();
}
