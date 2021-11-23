#ifndef LOCALISATIONPROTOCOLPF_HPP
#define LOCALISATIONPROTOCOLPF_HPP
#include "baseProtocol.hpp"
#include <map>

typedef struct MeasurementResults {int b1Dist; int b2Dist; int BeaconMasterDist; } MeasurementResults;//for  beaconMaster

class LocalisationProtocolPf: public BaseProtocol {
public:
    //roleInProtocol::Responder is only usable for S3Behaviour, pointer to ownerBehaviour is cast to S3beh
LocalisationProtocolPf(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh);
static constexpr int measureRetries = 3;
static constexpr int measureResWaitTicks = 2;
static constexpr int finalResultWaitTicks = 2;

virtual void start();
virtual bool tick();

static bool calculateXYFromDistances(int a, int b,int r,int R, float* points);
static float checkPointsWithThirdBeacon(int a, int b, int r, float* points);// returns difference between third beacons measurement and 1-2 closest calculated point
vector<int> knownHosts ={10,12,14,15};// hardcoded list of id for measurements, for simplicity
vector<int> result;// will contain cordinates if protocol result is sucessful
float findTwoClosestValues(vector<float> l);
float calcAngleToExplorer(int dToB1, int dToB2, float angleToB1, float angleToB2, MeasurementResults *mr);
float calcAngleToExplorerBestTriangle(int dToB1, int dToB2, float angleToB1, float angleToB2, MeasurementResults *mr);
float calcMinAngleOfTriangle(int a, int b, int c);
float minSines(float f1, float f2);
float minSines2(float f1, float f2, float et);

void localise();

private:
int curTargetIndex;
int curTargetId;
int measureRetryCounter=0;
int measureWaitCounter=0;
int finalResultWaitCounter=0;
ProtocolStates nextStateOnPositeiveResult;
ProtocolStates nextStateOnNegativeResult;
Topics currentBeacon;
bool respondersTick();
bool initiatorstTick();


void startDistanceMeasurement(Topics beaconId,ProtocolStates nextStatePosResult,ProtocolStates nextStateNegResult);
std::map<int,MeasurementResults > measurementResults;
//std::map<int,PointF > coordinatesOfPeers;
int getMeasurementTargetId();
bool insertResult(Topics sender, MeasurementResults *mr, int result);
//void querryBeacons();//ask own type s1 for reply, to know how many are available

};





#endif //LOCALISATIONPROTOCOLPF_HPP
