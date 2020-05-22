#ifndef BEACONMANAGEMENTPROTOCOL_HPP
#define BEACONMANAGEMENTPROTOCOL_HPP
#include "baseS1ManagementProtocol.hpp"
#include <set>
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class BeaconManagementProtocol: public BaseS1ManagementProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
BeaconManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


virtual void start();
virtual bool tick();
int getUnusedBeaconId();
void sendChangeType(int robotId, VSMSubsystems s1NewType );
private:
//std::set<int> availableRobotsSet;
bool bOneIsFilled =false;
bool bTwoIsFilled =false;
bool bMasterIsFilled =false;
bool managerTick();
bool beaconTick();

void querryBeacons();//ask own type s1 for reply, to know how many are available

};





#endif //BEACONMANAGEMENTPROTOCOL_HPP
