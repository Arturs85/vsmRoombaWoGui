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
#include "s4Behaviour.hpp"
#include <iomanip>
#define POINT_COUNT_FOR_MAP 6

LocalisationProtocol::LocalisationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;

    switch (roleInProtocol) {
    case RoleInProtocol::BEACON_MASTER:
        ownerBeh->subscribeToTopic(Topics::BEACON_MASTER_IN);
        break;
    case RoleInProtocol::CLIENT:
        ownerBeh->subscribeToDirectMsgs();
        break;
    case RoleInProtocol::S4:
        ownerBeh->subscribeToTopic(Topics::TO_S4);
        state = ProtocolStates::BEACONS_DEPLOYED;// dont care for initial forming, start with waiting coordinates
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
    case RoleInProtocol::S4:
        return s4Tick();
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
   * @return first of two values in the list @param l that has closest sines
   */

float LocalisationProtocol::findTwoClosestValues(vector<float> l)
{

    float cand1=0,cand2=0;
    float minErrSoFar = std::numeric_limits<float>::max();
    for (int i = 0; i < l.size(); i++) {
        for (int j = i+1; j < l.size(); j++) {
            float err = std::abs(sin(l.at(i))-sin(l.at(j)));//compare sines
            if(err<minErrSoFar){
                cand1= l.at(i);
                cand2 = l.at(j);
                minErrSoFar=err;
            }
        }
    }
    cout<<" err From two triangles, degrees = "<<(asin(minErrSoFar))<<"\n";
    return (cand1);
}

float LocalisationProtocol::calcAngleToExplorer(int dToB1, int dToB2, float angleToB1,float angleToB2,MeasurementResults *mr)
{
    // TreeMap<AID, Double> distances)
    std::cout<<"lp cate angle to b1 "<<angleToB1<<" to b2 "<<angleToB2<<"\n";
    double distFromMB= mr->BeaconMasterDist;// this implies that BeaconsMaster must bee one of beacons

    double beta = TwoPointFormationProtocol::calcAngle(distFromMB,dToB1/10,mr->b1Dist);

    double gamma = TwoPointFormationProtocol::calcAngle(distFromMB,dToB2/10,mr->b2Dist);
    std::cout<<"beta "<<180*beta/PI<< "gamma "<<180*gamma/PI<<"\n";
    vector<float> possibleAngles;
    possibleAngles.push_back((angleToB1+beta));
    possibleAngles.push_back((angleToB1-beta));
    possibleAngles.push_back((angleToB2+gamma));
    possibleAngles.push_back((angleToB2-gamma));
    std::cout<<"possible angles a1a  "<<possibleAngles.at(0)<<"a1b "<<possibleAngles.at(1)<<"a2a "<<possibleAngles.at(2)<<"a2b "<<possibleAngles.at(3)<<"\n";

    float closestVal= findTwoClosestValues(possibleAngles);
    return closestVal;//converting to radians
}

float LocalisationProtocol::calcAngleToExplorerBestTriangle(int dToB1, int dToB2, float angleToB1, float angleToB2, MeasurementResults *mr)
{
    double distFromMB= mr->BeaconMasterDist;// this implies that BeaconsMaster must bee one of beacons
float t1Min =  calcMinAngleOfTriangle(distFromMB,dToB1/10,mr->b1Dist);
float t2Min =  calcMinAngleOfTriangle(distFromMB,dToB2/10,mr->b2Dist);

float alfa1=0;
float alfa2 =0;
float beta1 =0;
float beta2 =0;

if(t1Min > t2Min){//triangle with b1 is better
float alfa =TwoPointFormationProtocol::calcAngle(distFromMB,dToB1/10,mr->b1Dist);
float beta = TwoPointFormationProtocol::calcAngle(distFromMB,dToB2/10,mr->b2Dist);
alfa1 = angleToB1+alfa;
alfa2 = angleToB1-alfa;
beta1 = angleToB2+beta;
beta2 = angleToB2-beta;

}else{
    float beta =TwoPointFormationProtocol::calcAngle(distFromMB,dToB1/10,mr->b1Dist);
    float alfa = TwoPointFormationProtocol::calcAngle(distFromMB,dToB2/10,mr->b2Dist);
    alfa1 = angleToB2+alfa;
    alfa2 = angleToB2-alfa;
    beta1 = angleToB1+beta;
    beta2 = angleToB1-beta;

}
float bestDiffToAlfa1 = minSines(alfa1-beta1,alfa1-beta2);//compare by sines
float bestDiffToAlfa2 = minSines(alfa2-beta1,alfa2-beta2);

if(bestDiffToAlfa1<bestDiffToAlfa2)
    return alfa1;
else
    return alfa2;

}


float LocalisationProtocol::calcMinAngleOfTriangle(int a, int b, int c)
{
  double alfa = TwoPointFormationProtocol::calcAngle(b,c,a);
  double beta = TwoPointFormationProtocol::calcAngle(a,c,b);
double gamma = PI/2-alfa-beta;

if(alfa<beta && alfa<gamma) return alfa;
if(beta<alfa && beta<gamma) return beta;
return gamma;
}

float LocalisationProtocol::minSines(float f1, float f2)
{
    if(std::abs(std::sin(f1))>std::abs(std::sin(f2)))
        return f2;
    else
        return f1;
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
            std::cout<<"lp meas res size "<<measurementResults.size()<<"\n";

        }
        MeasurementResults mr = measurementResults.at(id);
        bool isFilled =insertResult((Topics)res->sender,&mr,dist);// todo check if sender number realy contains topic number
        measurementResults.at(id)=mr;
        if(isFilled){//calculate xy and send to client and erease entry

            double angleToB1 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->angleToFirstRobot;
            double angleToB2 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->angleToSecondRobot;
            int distToB1 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->measuredDistToFirst[3];
            int distToB2 = ((BeaconMasterBehaviour*)behaviour)->thirdBeaconFormationProtocol->measuredDistToSecond[3];
//            float angleToExplorer = calcAngleToExplorer(distToB1,distToB2,angleToB1,angleToB2,&mr);
            float angleToExplorer = calcAngleToExplorerBestTriangle(distToB1,distToB2,angleToB1,angleToB2,&mr);

            std::cout<<"lp angle to exp "<<angleToExplorer<<"\n";

            int x = mr.BeaconMasterDist*std::cos(angleToExplorer);
            int y = mr.BeaconMasterDist*std::sin(angleToExplorer);
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
            std::cout<<std::setprecision (15)<<RoombaAgent::getSystemTimeSec()<<" localisation done x "<<result.at(0)<<" y "<<result.at(1) <<"\n";
            //resend coordinates to s4
            VSMMessage resultXY(behaviour->owner->id,Topics::TO_S4,MessageContents::CORDINATES_XY,res->content);
            behaviour->owner->sendMsg(resultXY);

            //erease older msgs
            VSMMessage* res2 =0;
            do{
                res2 = behaviour->receive(MessageContents::CORDINATES_XY);
            }while(res2 != 0);

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

bool LocalisationProtocol::s4Tick()
{
    switch (state) {
    case ProtocolStates::BEACONS_DEPLOYED:{
        // listen point messages
        VSMMessage* res = behaviour->receive(MessageContents::CORDINATES_XY);
        if(res!=0){// add poin to the list
            result = BaseProtocol::stringTointVector(res->content);

            int size =  ((S4Behaviour*)behaviour)->addPointToMap(result.at(0),result.at(1));
            if(size>POINT_COUNT_FOR_MAP){
                state = ProtocolStates::REGROUPING;
                ((S4Behaviour*)behaviour)->InitiateBeaconsRegroup();// triggers operation management protocol to regroup beacons
                std::cout<<"lp s4 map is filled ";
            }
        }
    }
        break;
    case ProtocolStates::REGROUPING:{//todo next states


    }
        break;
    default:
        break;
    }

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
    std::cout<<"lp bm inserting dist at "<<(int)sender<<" res "<<result<<"\n";
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
    std::cout<<"mr b1"<<mr->b1Dist<<"mr b2"<<mr->b2Dist<<"mr bm"<<mr->BeaconMasterDist<<"\n";

    if(mr->b1Dist!=0 && mr->b2Dist!= 0 && mr->BeaconMasterDist!=0 )
        return true;
    else return false;
}




