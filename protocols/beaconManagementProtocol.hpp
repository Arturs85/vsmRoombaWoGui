#ifndef BEACONMANAGEMENTPROTOCOL_HPP
#define BEACONMANAGEMENTPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class BeaconManagementProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
BeaconManagementProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);


virtual void start();
virtual bool tick();

private:

bool senderTick();
bool receiverTick();


};





#endif //BEACONMANAGEMENTPROTOCOL_HPP
