#ifndef S2BASEBEHAVIOUR_HPP
#define S2BASEBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"

class ControlValueProtocol;
class BaseS1ManagementProtocol;
class S1ExchangeProtocol;

enum S2Types{BEACONS, EXPLORERS,SIZE_OF_THIS_ENUM};

class S2BaseBehavior : public BaseCommunicationBehaviour{

public:
S2BaseBehavior(RoombaAgent *roombaAgent);
int lastControlValue=0;
int lastS1Count =0;
S2Types s2type;
virtual int getS1IdForGiveaway();
BaseS1ManagementProtocol* s1ManagementProtocol;

protected:
void behaviourStep();
S1ExchangeProtocol* s1ExchangeProtocol;

private:
ControlValueProtocol* controlValueProtocol;




};





#endif //S2BASEBEHAVIOUR_HPP
