#ifndef S2EXPLORERSBEHAVIOUR_HPP
#define S2EXPLORERSBEHAVIOUR_HPP
#include "roombaAgent.hpp"
#include "s2BaseBehaviour.hpp"

class ExplorerManagementProtocol;
class OperationsManagementProtocol;

class S2ExplorersBehaviour: public S2BaseBehavior{// waits for requests from second beacon by TwoPoinFormation protocol
public:
    RoombaAgent* ra;
   // TwoPointFormationProtocol* twoPointFormationProtocol;
    S2ExplorersBehaviour(RoombaAgent* roombaAgent);
 ExplorerManagementProtocol* explorerManagementProtocol;
OperationsManagementProtocol* operationsManagementProtocol;
int getS1IdForGiveaway();
void enterIdleState();
void enterExploringState();
private:
    void behaviourStep();



};






#endif //S2EXPLORERSBEHAVIOUR_HPP
