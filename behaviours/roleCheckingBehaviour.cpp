#include "roleCheckingBehaviour.hpp"
#include <unistd.h>

RoleCheckingBehaviour::RoleCheckingBehaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    type= VSMSubsystems::ROLE_CHECKING;
    roleCheckingProtocol = new RoleCheckingProtocol(RoleInProtocol::INITIATOR,this);
    roleCheckingProtocol->start();
   
}

void RoleCheckingBehaviour::behaviourStep()
{
//	std::cout<<" rcb msg size "<<msgDeque.size()<<"\n";

bool hasEnded =roleCheckingProtocol->tick();
//std::cout<<"rcp tick returned\n";
}

void RoleCheckingBehaviour::askS3ForRoles()
{

}
