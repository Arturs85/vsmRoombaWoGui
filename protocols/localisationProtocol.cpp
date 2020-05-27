#include "localisationProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include "beaconMasterBehaviour.hpp"
#include "thirdBeaconFormationProtocol.hpp"
#include "twoPointFormationProtocol.hpp"
#include <cmath>
#include <limits>


LocalisationProtocol::LocalisationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;

    switch (roleInProtocol) {
    case RoleInProtocol::BEACON_MASTER:
        ownerBeh->subscribeToTopic(Topics::BEACON_MASTER_IN);
        break;
    case RoleInProtocol::CLIENT:

        break;
    default:
        break;
    }

}

void LocalisationProtocol::start()
{

}



bool LocalisationProtocol::tick()//beacon listens measurements and sends them to master (automaticly via uwblistener), no need for tjhis protocol for them
{
    switch (roleInProtocol) {
    case RoleInProtocol::BEACON_MASTER:
        return beaconMsterTick();
        break;
    case RoleInProtocol::CLIENT:
        return clientTick();
        break;
    default:
        break;
    }
}
/**
 * a - xcord of beacon
 * b- ycord of beacon
 * r- dist measurement between beacon and robot
 * R- dist measurement between beaconMaster and robot
 *
 */


bool LocalisationProtocol::calculateXYFromDistances(int a, int b,int r,int R, float* points)
{
    if(a==0)a=1;// avoid division by zero

    float x1Out,y1Out,x2Out,y2Out;
    float c = -(r*r-R*R-a*a-b*b/2/a);
    float dskr  = 4*c*c*b*b/a/a-(4*(b*b/a/a+1)*(c*c-R*R));
    if(dskr<0){}//no usable solution, this means that there are not coincident points on both circles, most likely target is between beacons, return
    return false;
    float y1 = (2*c*b/a+std::sqrt(dskr))/(2*(b*b/a/a+1));
    float y2 = (2*c*b/a-std::sqrt(dskr))/(2*(b*b/a/a+1));

    //float h1 =R*R-y1*y1;//todo check if positive before sqrt
    //float h2 =R*R-y2*y2;

    float x1Aty1 = std::sqrt(R*R-y1*y1);
    float x2Aty1 = -std::sqrt(R*R-y1*y1);
    float x1Aty2 = std::sqrt(R*R-y2*y2);
    float x2Aty2 = -std::sqrt(R*R-y2*y2);

    //check which x solves firsr eq
    // for y1 value
    float targetValY1 = r*r-(y1-b)*(y1-b);
    float resX1Y1 = targetValY1-(x1Aty1-a)*(x1Aty1-a);
    float resX2Y1 = targetValY1-(x2Aty1-a)*(x2Aty1-a);

    if(std::abs(resX1Y1)<std::abs(resX2Y1))
        x1Out = x1Aty1;
    else
        x1Out = x2Aty1;

    // for y1 value
    float targetValY2 = r*r-(y2-b)*(y2-b);
    float resX1Y2 = targetValY2-(x1Aty2-a)*(x1Aty2-a);
    float resX2Y2 = targetValY2-(x2Aty2-a)*(x2Aty2-a);

    if(std::abs(resX1Y2)<std::abs(resX2Y2))
        x2Out = x1Aty2;
    else
        x2Out = x2Aty2;

    points[0]=x1Out;
    points[1]=y1;
    points[2]=x2Out;
    points[3]=y2;
    return true;
}

/**
 * a - xcord of beacon
 * b- ycord of beacon
 * r- dist measurement between beacon and robot
 * points- cordinates to compare in form x1,y1,x2,y2
 */
float LocalisationProtocol::checkPointsWithThirdBeacon(int a, int b, int r, float *points)
{
    float diffH1 = r*r -(points[0]-a)*(points[0]-a)-(points[1]-b)*(points[1]-b);
    float diffH2 = r*r -(points[2]-a)*(points[2]-a)-(points[3]-b)*(points[3]-b);

    if(std::abs(diffH1)>std::abs(diffH2)){
        points[0]=points[2];
        points[1]=points[3];// first two elements of points will contain x,y of closest match
        return diffH2;
    }
    return diffH1;
}



/**
   *
   * @param l
   * @return average of two values in the list @param l that are closest
   */

float LocalisationProtocol::findTwoClosestValues(vector<float> l)
{

    float cand1=0,cand2=0;
    float minErrSoFar = std::numeric_limits<float>::max();
    for (int i = 0; i < l.size(); i++) {
        for (int j = i+1; j < l.size(); j++) {
            float err = std::abs(l.at(i)-l.at(j));
            if(err<minErrSoFar){
                cand1= l.at(i);
                cand2 = l.at(j);
                minErrSoFar=err;
            }
        }
    }
    cout<<" err From two triangles, degrees = "<<(minErrSoFar)<<"\n";
    return (cand1+cand2)/2;
}

float LocalisationProtocol::calcAngleToExplorer(int dToB1, int dToB2, float angleToB1,float angleToB2,MeasurementResults *mr)
{
    // TreeMap<AID, Double> distances)
    double distFromMB= mr->BeaconMasterDist;// this implies that BeaconsMaster must bee one of beacons

    double beta = TwoPointFormationProtocol::calcAngle(distFromMB,dToB1,mr->b1Dist);
    double gamma = TwoPointFormationProtocol::calcAngle(distFromMB,dToB2,mr->b2Dist);
    vector<float> possibleAngles;
    possibleAngles.push_back(angleToB1+beta);
    possibleAngles.push_back(angleToB1-beta);
    possibleAngles.push_back(angleToB2+gamma);
    possibleAngles.push_back(angleToB2-gamma);
    return findTwoClosestValues(possibleAngles);
}




bool LocalisationProtocol::beaconMsterTick()// listens measurements, and waits timeout to receive measurements from beacons, then calculates xy and sends it to client
{
    VSMMessage* res = behaviour->receive(MessageContents::DISTANCE_MEASUREMENT_AND_ID);
    if(res!= 0){// add received measurement to list
        vector<int> data = BaseProtocol::stringTointVector(res->content);
        int id = data.at(1);
        int dist = data.at(0);
        auto it = measurementResults.find(id);
        if (it != measurementResults.end()){//there is entry, add measurement and check wether all measurements are present

        }else{//create neew entry
            measurementResults.insert( std::pair<int,MeasurementResults>(id,{0,0,0}) );

        }
        MeasurementResults mr = measurementResults.at(id);
        bool isFilled =insertResult((Topics)res->senderNumber,&mr,dist);// todo check if sender number realy contains topic number
        if(isFilled){//calculate xy and send to client and erease entry

            double angleToB1 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->angleToFirstRobot;
            double angleToB2 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->angleToSecondRobot;
            int distToB1 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->measuredDistToFirst[3];
            int distToB2 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->measuredDistToSecond[3];
            float angleToExplorer = calcAngleToExplorer(distToB1,distToB2,angleToB1,angleToB2,&mr);
            int x = mr.BeaconMasterDist*std::cos(angleToExplorer);
            int y = mr.BeaconMasterDist*std::cos(angleToExplorer);
            vector<int> resVect{x,y};
            std::string resString = BaseProtocol::intVectorToString(resVect);
            VSMMessage resultXY(behaviour->owner->id,id,MessageContents::CORDINATES_XY,resString);
            behaviour->owner->sendMsg(resultXY);

            auto it = measurementResults.find(id);
            measurementResults.erase(it);
        }
    }
}



bool LocalisationProtocol::clientTick()//  wait for final result timeout
{
    std::cout<<"lp client tick, state "<<(int)state<<"\n";

    switch (state) {

    case ProtocolStates::MASTER_MEAS_RECEIVED:{// measure distance to beacon 2
        startDistanceMeasurement(Topics::BEACON_TWO_IN,ProtocolStates::B2_MEAS_RECEIVED,ProtocolStates::TIMEOUT);

    }break;

    case ProtocolStates::B2_MEAS_RECEIVED:{// measure distance to beacon 1
        startDistanceMeasurement(Topics::BEACON_ONE_IN,ProtocolStates::B1_MEAS_RECEIVED,ProtocolStates::TIMEOUT);

    }break;
    case ProtocolStates::B1_MEAS_RECEIVED:{// wait for result from beacon master
        VSMMessage* res = behaviour->receive(MessageContents::CORDINATES_XY);
        if(res!=0){

            result = BaseProtocol::stringTointVector(res->content);
           std::cout<<"localisation done x "<<result.at(0)<<" y "<<result.at(1) <<"\n";
            wasSuccessful = true;
            return true;
        }
        finalResultWaitCounter++;
        if(finalResultWaitCounter>finalResultWaitTicks){
            std::cout<<"localisation client failed to receive cords, stopping\n";
            wasSuccessful = false;
            return true;
        }

    }break;

    case ProtocolStates::WAITING_DIST_MEASURE_RESULT:{
        measureWaitCounter++;
        VSMMessage* res = behaviour->receive(MessageContents::DISTANCE_MEASUREMENT);
        if(res!= 0){
            int latestMeasurement = 10*std::stoi(res->content);//multiplying by ten to switch to mm
            std::cout<<"integer meas. result "<<latestMeasurement<<"\n";
            state = nextStateOnPositeiveResult; //
        }else if(measureWaitCounter>measureResWaitTicks){// if result is not received within timeout(ticks) then retry measurement
            state = ProtocolStates::TIMEOUT;//
            measureWaitCounter=0;
        }
    }
        break;

    case ProtocolStates::TIMEOUT:{
        measureRetryCounter++;
        std::cout<<"localisation timeout "<<measureRetryCounter<<"\n";
        if(measureRetryCounter<=measureRetries){
            startDistanceMeasurement(currentBeacon,nextStateOnPositeiveResult,nextStateOnNegativeResult);
            break;
        }
        std::cout<<"localisation failed to measure dist, stopping";
        wasSuccessful = false;
        return true;
    }   break;

    default:
        break;
    }
return false;
}

void LocalisationProtocol::localise()//send dist measurement request to beacon master- initiates localisation process
{
    startDistanceMeasurement(Topics::BEACON_MASTER_IN,ProtocolStates::MASTER_MEAS_RECEIVED,ProtocolStates::TIMEOUT);
}

void LocalisationProtocol::startDistanceMeasurement(Topics beaconId,ProtocolStates nextStatePosResult,ProtocolStates nextStateNegResult)
{
    currentBeacon = beaconId;
    //latestMeasurement=0;//dont care about value, we use this msg as confirmation to proceed with next measurement or step
    nextStateOnNegativeResult = nextStateNegResult;// assign these values directly prior this call?
    nextStateOnPositeiveResult = nextStatePosResult;
    behaviour->owner->uwbMsgListener.addToRangingInitDeque((int)beaconId);//send measurement initation
    state = ProtocolStates::WAITING_DIST_MEASURE_RESULT;
}

bool LocalisationProtocol::insertResult(Topics sender,MeasurementResults* mr, int result)//returns true if mr is filled after insertion
{
    switch (sender) {
    case Topics::BEACON_MASTER_IN:
        mr->BeaconMasterDist=result;
        break;
    case Topics::BEACON_TWO_IN:
        mr->b2Dist=result;
        break;
    case Topics::BEACON_ONE_IN:
        mr->b1Dist=result;
        break;
    default:
        break;
    }
    if(mr->b1Dist!=0 && mr->b2Dist!= 0 && mr->BeaconMasterDist!=0 )
        return true;
    else return false;
}




