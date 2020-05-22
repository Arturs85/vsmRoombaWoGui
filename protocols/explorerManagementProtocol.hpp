#ifndef EXPLORERMANAGEMENTPROTOCOL_HPP
#define EXPLORERMANAGEMENTPROTOCOL_HPP
#include "baseS1ManagementProtocol.hpp"
#include <set>
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class ExplorerManagementProtocol: public BaseS1ManagementProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
ExplorerManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


virtual void start();
virtual bool tick();
int getUnusedBeaconId();
void sendChangeType(int robotId, VSMSubsystems s1NewType );
void sendStopExploring();
private:
//std::set<int> availableExplorersSet;// move to Superclass?
bool bOneIsFilled =false;
bool bTwoIsFilled =false;
bool bMasterIsFilled =false;
bool managerTick();
bool explorerTick();

void querryExplorers();//ask own type s1 for reply, to know how many are available

};





#endif //EXPLORERMANAGEMENTPROTOCOL_HPP
