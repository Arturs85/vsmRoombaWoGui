#ifndef S1EXCHANGEPROTOCOL_HPP
#define S1EXCHANGEPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class S1ExchangeProtocol: public BaseProtocol {
public:
S1ExchangeProtocol(BaseCommunicationBehaviour *ownerBeh);

vector<int> cVals;
virtual void start();
virtual bool tick();

void askS1(int cValue);

private:
int tickCount=0;
bool senderTick();
bool receiverTick();


};





#endif //S1EXCHANGEPROTOCOL_HPP
