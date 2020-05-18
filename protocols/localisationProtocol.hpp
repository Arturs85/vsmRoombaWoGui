#ifndef LOCALISATIONPROTOCOL_HPP
#define LOCALISATIONPROTOCOL_HPP
#include "baseProtocol.hpp"
#include <map>

typedef struct MeasurementResults {int b1Dist; int b2Dist; int BeaconMasterDist; } MeasurementResults;//for  beaconMaster

class LocalisationProtocol: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
LocalisationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);
static constexpr int measureRetries = 3;
static constexpr int measureResWaitTicks = 2;
static constexpr int finalResultWaitTicks = 2;

virtual void start();
virtual bool tick();

static bool calculateXYFromDistances(int a, int b,int r,int R, float* points);
static float checkPointsWithThirdBeacon(int a, int b, int r, float* points);// returns difference between third beacons measurement and 1-2 closest calculated point

vector<int> result;// will contain cordinates if protocol result is sucessful

private:

int measureRetryCounter=0;
int measureWaitCounter=0;
int finalResultWaitCounter=0;
ProtocolStates nextStateOnPositeiveResult;
ProtocolStates nextStateOnNegativeResult;
Topics currentBeacon;
bool beaconMsterTick();
bool beaconTick();
bool clientTick();
void localise();
void startDistanceMeasurement(Topics beaconId,ProtocolStates nextStatePosResult,ProtocolStates nextStateNegResult);
std::map<int,MeasurementResults > measurementResults;
bool insertResult(Topics sender, MeasurementResults *mr, int result);
//void querryBeacons();//ask own type s1 for reply, to know how many are available

};





#endif //LOCALISATIONPROTOCOL_HPP