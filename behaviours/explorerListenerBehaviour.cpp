#include "explorerListenerBehaviour.hpp"
#include "localisationProtocol.hpp"
#include "explorerManagementProtocol.hpp"

// add this behaviour when unit becomes of explore type, so it can listen to commands from s2


ExplorerListenerBehaviour::ExplorerListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    explorerManagementProtocol  = new ExplorerManagementProtocol(RoleInProtocol::EXPLORER,this);
    localisationProtocol=new LocalisationProtocol(RoleInProtocol::CLIENT,this);
}

void ExplorerListenerBehaviour::startExploring()
{
    std::cout<<"elb explorer starting explore\n";
    explorerState=ExplorerStates::EXPLORING;
    localise();//for testing
}

void ExplorerListenerBehaviour::stopExploring()
{
    explorerState=ExplorerStates::IDLE;//todo improve
    std::cout<<"elb explorer stopping explore\n";

}


void ExplorerListenerBehaviour::behaviourStep()
{
     BaseCommunicationBehaviour::logKeypoints("emp tick called\n");
    explorerManagementProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("emp tick returned\n");
    //manage driving
    switch (explorerState) {
    case ExplorerStates::IDLE:

        break;

    case ExplorerStates::MEASURING_XY:{
        bool finished = localisationProtocol->tick();
        if(finished){//
            if(localisationProtocol->wasSuccessful)
            {// xy are available, use them for mapping or direction calculation, if this is second measurement after traveling dist

            }
        }
    }
        break;
    case ExplorerStates::EXPLORING:

        break;
    default:
        break;
    }


}

//add localisation protocol as client
void ExplorerListenerBehaviour::localise()
{
    localisationProtocol->localise();
    explorerState = ExplorerStates::MEASURING_XY;
}
