#ifndef S2EXPLORERSBEHAVIOUR_HPP
#define S2EXPLORERSBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "s2BaseBehaviour.hpp"

class BeaconManagementProtocol;
class OperationsManagementProtocol;

class S2ExplorersBehaviour: public S2BaseBehavior{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    S2ExplorersBehaviour(RoombaAgent* roombaAgent);
 //BeaconManagementProtocol* beaconManagementProtocol;
OperationsManagementProtocol* operationsManagementProtocol;
private:
    void behaviourStep();



};






#endif //S2EXPLORERSBEHAVIOUR_HPP
