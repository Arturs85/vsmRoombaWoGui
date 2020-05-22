#ifndef S2BASEBEHAVIOUR_HPP
#define S2BASEBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"

class ControlValueProtocol;
class BaseS1ManagementProtocol;
class S1ExchangeProtocol;
class RoleCheckingProtocol;

enum S2Types{BEACONS, EXPLORERS,SIZE_OF_THIS_ENUM};

class S2BaseBehavior : public BaseCommunicationBehaviour{

public:
S2BaseBehavior(RoombaAgent *roombaAgent);
~S2BaseBehavior();

int lastControlValue=0;
int lastS1Count =0;
S2Types s2type;
virtual int getS1IdForGiveaway();
BaseS1ManagementProtocol* s1ManagementProtocol;
S1ExchangeProtocol* s1ExchangeProtocol;

protected:
void behaviourStep();

private:
ControlValueProtocol* controlValueProtocol;
RoleCheckingProtocol* roleCheckingProtocol;



};





#endif //S2BASEBEHAVIOUR_HPP
