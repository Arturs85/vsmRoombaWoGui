#ifndef BASEPROTOCOL_HPP
#define BASEPROTOCOL_HPP
#include "baseCommunicationBehaviour.hpp"

enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};


class BaseProtocol{
public:
    BaseProtocol(BaseCommunicationBehaviour* ownerBeh);

protected:

    ProtocolStates state;
BaseCommunicationBehaviour* behaviour;

private:
bool wasSuccessful =false;
virtual bool tick();
virtual void start();



};





#endif //BASEPROTOCOL_HPP
