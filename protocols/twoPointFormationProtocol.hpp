#ifndef TWOPOINTFORMATIONROTOCOL_HPP
#define TWOPOINTFORMATIONROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class TwoPointFormationProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
TwoPointFormationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);

int stillBeaconId=0;
virtual void start();
virtual bool tick();

static double calculateRelativeAngle(double mes1, double mes2, double odoDist);
static double calcThirdSide(double a, double b, double angleRad);
static double calcAngle(double a,double b,double c);

static constexpr int measureRetries = 3;

private:
int measureRetryCounter=0;
int measuredDist[4];
int latestMeasurement=0;
double relativeAngleH1;
   double relativeAngleH2;
   double dirBeforeTurn;
   double angleToOtherRobot;
ProtocolStates nextStateOnPositeiveResult;
ProtocolStates nextStateOnNegativeResult;
bool standingBeaconTick();
bool movingBeaconTick();
void enterState(ProtocolStates stateToEnter);// for moving beacon
void startDistanceMeasurement(int id, ProtocolStates nextStatePosResult, ProtocolStates nextStateNegResult);

};





#endif //TWOPOINTFORMATIONROTOCOL_HPP
