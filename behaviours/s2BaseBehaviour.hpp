#ifndef S2BASEBEHAVIOUR_HPP
#define S2BASEBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"

class ControlValueProtocol;
enum S2Types{BEACONS, EXPLORERS};

class S2BaseBehavior : public BaseCommunicationBehaviour{

public:
S2BaseBehavior(RoombaAgent *roombaAgent);

protected:
void behaviourStep();


private:
ControlValueProtocol* controlValueProtocol;




};





#endif //S2BASEBEHAVIOUR_HPP
