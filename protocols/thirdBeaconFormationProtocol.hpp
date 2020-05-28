#ifndef THIRDBEACONFORMATIONPROTOCOL_HPP
#define THIRDBEACONFORMATIONPROTOCOL_HPP
#include "baseProtocol.hpp"
#include <set>
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class ThirdBeaconFormationProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
ThirdBeaconFormationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

int stillBeacon1Id=0;
int stillBeacon2Id=0;

virtual void start();
virtual bool tick();

std::set<int> stillBeaconResponders;
static constexpr int measureRetries = 3;
static constexpr int measureResWaitTicks = 4;// wait long enough to don't miss reply, because it can than be missreceived as next measurement result
static constexpr int queryRetries = 3;
static constexpr int replyResWaitTicks = 2;


static double calcAngleAfterTurnAndMove(double prevRelatAngle, double turnAngle, double travel, double measAfter, double measBefore);
double angleToFirstRobot;
double angleToSecondRobot;
int measuredDistToFirst[4];
int measuredDistToSecond[4];

private:
int measureRetryCounter=0;
int measureWaitCounter=0;
int queryRetryCounter=0;
int replyWaitCounter=0;


int latestMeasurement=0;
int latestMeasuredBeaconId=0;

double relativeAngleH1First;
double relativeAngleH2First;
double relativeAngleH1Second;
double relativeAngleH2Second;
double relativeAngleH1AfterTurnF=0;
double relativeAngleH2AfterTurnF=0;
double relativeAngleH1AfterTurnS=0;
double relativeAngleH2AfterTurnS=0;
double finalDistance;
double finalAngle;
double odoBeforeTravel =0;

double dirBeforeTurn;


ProtocolStates nextStateOnPositeiveResult;
ProtocolStates nextStateOnNegativeResult;
bool standingBeaconTick();
bool movingBeaconTick();
void enterState(ProtocolStates stateToEnter);// for moving beacon
void startDistanceMeasurement(int id, ProtocolStates nextStatePosResult, ProtocolStates nextStateNegResult);

};





#endif //THIRDBEACONFORMATIONPROTOCOL_HPP
