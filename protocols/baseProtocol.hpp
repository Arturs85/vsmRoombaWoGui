#ifndef BASEPROTOCOL_HPP
#define BASEPROTOCOL_HPP
#include "baseCommunicationBehaviour.hpp"

enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};
enum RoleInProtocol{INITIATOR, RESPONDER};


class BaseProtocol{

public:
    BaseProtocol(BaseCommunicationBehaviour* ownerBeh);
RoleInProtocol roleInProtocol;
protected:
int waitTicksCounter =0;
    ProtocolStates state;
BaseCommunicationBehaviour* behaviour;
bool wasSuccessful =false;

private:
virtual bool tick();
virtual void start();



};





#endif //BASEPROTOCOL_HPP
