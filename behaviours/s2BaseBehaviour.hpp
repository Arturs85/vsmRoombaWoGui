#ifndef S2BASEBEHAVIOUR_HPP
#define S2BASEBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"

class ControlValueProtocol;

class S2BaseBehavior : public BaseCommunicationBehaviour{

public:
S2BaseBehavior(RoombaAgent *roombaAgent);

private:
ControlValueProtocol* controlValueProtocol;

void behaviourStep();



};





#endif //S2BASEBEHAVIOUR_HPP
