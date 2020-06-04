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
void startReformation(vector<int> cords);
private:
//std::set<int> availableRobotsSet;
bool bOneIsFilled =false;
bool bTwoIsFilled =false;
bool bMasterIsFilled =false;
bool managerTick();
bool beaconTick();
bool targetGoerBeaconTick();
int currentRobotId =0;
int askingIntervalCounter=0;
void querryBeacons();//ask own type s1 for reply, to know how many are available
std::set<int> regroupingBeaconsSet;
vector<int> cords;//for regrouping beacons
};





#endif //BEACONMANAGEMENTPROTOCOL_HPP
