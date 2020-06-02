#ifndef S2BEACONSBEHAVIOUR_HPP
#define S2BEACONSBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "s2BaseBehaviour.hpp"

class BeaconManagementProtocol;
class OperationsManagementProtocol;

class S2BeaconsBehaviour: public S2BaseBehavior{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    S2BeaconsBehaviour(RoombaAgent* roombaAgent);
 BeaconManagementProtocol* beaconManagementProtocol;
OperationsManagementProtocol* operationsManagementProtocol;
int getS1IdForGiveaway();
void startRegroupingBeacons(vector<int> cords);

private:
    void behaviourStep();



};






#endif //S2BEACONSBEHAVIOUR_HPP
