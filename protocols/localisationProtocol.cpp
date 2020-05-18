#include "localisationProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "beaconOneBehaviour.hpp"
#include "s2BeaconsBehaviour.hpp"
#include <cmath>



LocalisationProtocol::LocalisationProtocol(RoleInProtocol roleInProtocol, BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    this->roleInProtocol = roleInProtocol;

}



bool LocalisationProtocol::tick()// todo modify from source copy
{

    return false;
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
            measurementResults.insert( std::pair<int,MeasurementResults>(id,{0,0,0}) );)

        }
        MeasurementResults mr = measurementResults.at(id);
        bool isFilled =insertResult((Topics)res->senderNumber,&mr,dist);// todo check if sender number realy contains topic number
        if(isFilled){//calculate xy and send to client and erease entry


            auto it = measurementResults.find(id);
            measurementResults.erase(it);
        }
    }
}

bool LocalisationProtocol::beaconTick()//listens measurements and sends them to master (automaticly via uwblistener)
{

}

bool LocalisationProtocol::clientTick()//  wait for final result timeout
{
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
            std::cout<<"localisation done\n";
            result = BaseProtocol::stringTointVector(res->content);
            wasSuccessful = true;
            return true;
        }
        finalResultWaitCounter++;
        if(finalResultWaitCounter>finalResultWaitTicks){
            std::cout<<"localisation client failed to receive cords, stopping";
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




