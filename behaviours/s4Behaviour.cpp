#include "s4Behaviour.hpp"
#include "roleCheckingProtocol.hpp"
#include "controlValueProtocol.hpp"
#include "operationsManagementProtocol.hpp"
#include "roombaAgent.hpp"



S4Behaviour::S4Behaviour(RoombaAgent *owner):BaseCommunicationBehaviour(owner)
{
    type=VSMSubsystems::S4;
     

   
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S4,this);
    operationsManagementProtocol->start();
}




void S4Behaviour::behaviourStep()
{
    operationsManagementProtocol->tick();
 
}
