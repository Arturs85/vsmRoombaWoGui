#ifndef BASES1MANAGEMENTPROTOCOL_HPP
#define BASES1MANAGEMENTPROTOCOL_HPP
#include "baseProtocol.hpp"
#include <set>
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class BaseS1ManagementProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
BaseS1ManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


virtual void start();
virtual bool tick();
virtual int getUnusedBeaconId();
void sendChangeType(int robotId, std::string vSMSubsystemsS1NewTypeString );
std::set<int> availableRobotsSet;

protected:
std::set<int> usedRobots; //use set or vctor?

std::set<int> getUnusedRobotsSet();

bool managerTick();
bool beaconTick();

//void querryBeacons();//ask own type s1 for reply, to know how many are available

};





#endif //BASES1MANAGEMENTPROTOCOL_HPP
