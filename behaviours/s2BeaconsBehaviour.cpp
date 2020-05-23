#include "s2BeaconsBehaviour.hpp"
#include "beaconManagementProtocol.hpp"
#include "operationsManagementProtocol.hpp"

S2BeaconsBehaviour::S2BeaconsBehaviour(RoombaAgent *roombaAgent):S2BaseBehavior(roombaAgent)
{
    this->ra = roombaAgent;
    s2type = S2Types::BEACONS;
    type = VSMSubsystems::S2_BEACONS;
    s1ManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::S2BEACON,this);
    operationsManagementProtocol = new OperationsManagementProtocol(RoleInProtocol::S2BEACON,this);
    operationsManagementProtocol->start();
}


void S2BeaconsBehaviour::behaviourStep()
{
    //	cout<<"bmp s2 inherited step called \n";

    S2BaseBehavior::behaviourStep();
    //cout<<"bmp s2 inherited step returned \n";

    ((BeaconManagementProtocol*)s1ManagementProtocol)->tick();
    BaseCommunicationBehaviour::logKeypoints("bmp s2 tick returned \n");

    operationsManagementProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("omp s2 tick returned \n");

}

int S2BeaconsBehaviour::getS1IdForGiveaway()
{

    //check bmp to see if there is posssible beacon to give away

    return s1ManagementProtocol->getUnusedBeaconId();
    //if(beaconManagementProtocol->)
    return 0;
}
