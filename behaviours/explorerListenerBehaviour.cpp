#include "explorerListenerBehaviour.hpp"
#include "localisationProtocol.hpp"

// add this behaviour when unit becomes of explore type, so it can listen to commands from s2


ExplorerListenerBehaviour::ExplorerListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    //beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::BEACON,this);
    localisationProtocol=new LocalisationProtocol(RoleInProtocol::CLIENT,this);
}

void ExplorerListenerBehaviour::startExploring()
{
    explorerState=ExplorerStates::EXPLORING;
    localise();//for testing
}


void ExplorerListenerBehaviour::behaviourStep()
{
    // beaconManagementProtocol->tick();
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
