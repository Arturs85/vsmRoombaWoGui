
#include "particlefilter.h"
#include <iostream>
#include <algorithm>
#include <iomanip>      // std::setprecision
#include "logfilesaverpf.hpp"
#include <sstream>
#include <float.h>
ParticleFilter::ParticleFilter()
{
    turnAngleDtribution = std::normal_distribution<double>(0.0,8.0/180*M_PI);// stddev radians
    linMovementDistribution = std::normal_distribution<double>(0.0,0.1);// stddev in part of distance i.e dist/10
    regenSpatialDist = std::normal_distribution<double>(0.0, 0.05/radiOfEarthForDegr); //0.1m stddev , use speed instead?
    initializeParticles(0,0);

}

//void ParticleFilter::onOdometry(Position2D position, Position2D deltaPosition){
//    //  std::cout<<"particleFilter onOdometry called "<<position.x<<std::endl;
//    moveParticles(deltaPosition.x,deltaPosition.y,deltaPosition.yaw);
//    addMovementNoise();

//}

void ParticleFilter::onOdometry(double dt){// for use wo actual odometry
    addLinearMovementNoise(dt);
}

void ParticleFilter::onDistance(double fx,double fy, double nx, double ny, double dist){


    // regenerate

    double maxDist = std::sqrt((avgParticle.x-fx)*(avgParticle.x-fx)+(avgParticle.y-fy)*(avgParticle.y-fy))+distMeasErr;
    double minDist = std::sqrt((avgParticle.x-nx)*(avgParticle.x-nx)+(avgParticle.y-ny)*(avgParticle.y-ny))-distMeasErr;

    double dev = (maxDist-minDist)/2;
    double midpointX = (nx+fx)/2;
    double midpointY = (ny+fy)/2;

    std::cout<<"min, max, dev "<<minDist<<" "<<maxDist<<" "<<dev<<" otherMidX "<<midpointX<< " oy=therMidY "<< midpointY<<std::endl;


    calcFitnessGeneric(midpointX,midpointY,dev,dist);
    regenerateParticles();
    calcAverageParticle();
    //write to file

    std::stringstream ss;

    for (int i = 0; i < particles.size(); ++i) {
        ss<<particles.at(i).x<<" "<<particles.at(i).y<<" "<<particles.at(i).direction<<std::endl;

    }
    ss<<"eol"<<std::endl;
    ss<<midpointX<<" "<<midpointY<<" "<<dev<<" "<<dist<<std::endl;
    ss<<"eoll"<<std::endl;
    LogFileSaverPf::logfilesaver.writeString(ss);
}


vector<int> ParticleFilter::getFarestAndNearestPoints( Position2D p) { // todo synchronosation needed on particles?
    double farestDistSaoFar = 0;
    int farestIndex = 0;
    double nearestDistSaoFar = DBL_MAX;// from float.h
    int nearestIndex = 0;

    for (int i = 0; i < particles.size(); i++) {
        int dx = particles.at(i).x - p.x;
        int dy = particles.at(i).y - p.y;
        double dis = std::sqrt(dx * dx + dy * dy);
        if (dis > farestDistSaoFar) {
            farestDistSaoFar = dis;
            farestIndex = i;
        }
        if (dis < nearestDistSaoFar) {
            nearestDistSaoFar = dis;
            nearestIndex = i;
        }
    }

    vector<int> res{(int)particles.at(nearestIndex).x, (int)particles.at(nearestIndex).y, (int)particles.at(farestIndex).x, (int)particles.at(farestIndex).y, 0};
    return res;

}

void ParticleFilter::turnParticles(double angSp, double dt ){
    

    for (int i = 0; i < particles.size(); i++) {
        //   double errYawSpDeg = (yawSpeedDtribution(generator));
        //std::cout<<errYawSpDeg<<" " ;
        //   particles.at(i).addToDirectionAndNormalize((errYawSpDeg+angSp)*dt*M_PI/180);

    }
    //std::cout<<std::endl;
}

void ParticleFilter::turnParticles(double angleRad ){


    for (int i = 0; i < particles.size(); i++) {
        double errYaw = (turnAngleDtribution(generator));
        //std::cout<<errYawSpDeg<<" " ;
        particles.at(i).addToDirectionAndNormalize((errYaw+angleRad));

    }
    //std::cout<<std::endl;
}
void ParticleFilter::moveForward(double dist)//
{
    // todo add noise
    for (int i = 0; i < particles.size(); i++) {
        double movementErr = linMovementDistribution(generator);
        particles.at(i).moveForward(dist+dist*movementErr);

    }
    std::stringstream ss;

    for (int i = 0; i < particles.size(); ++i) {
        ss<<particles.at(i).x<<" "<<particles.at(i).y<<" "<<particles.at(i).direction<<std::endl;

    }
    ss<<"eoll"<<std::endl;
    LogFileSaverPf::logfilesaver.writeString(ss);

}
void ParticleFilter::moveParticles(double dx, double dy, double dyaw)
{
    for (int i = 0; i < particles.size(); i++) {

        double dist =  std::sqrt(dx*dx+dy*dy);
        double travelAngle = particles.at(i).direction+dyaw/2;


        double fi = atan2(dy,dx);
        //double dxg = dx*cos(fi+particles.at(i).direction)/cos(fi);
        //double dyg = dy*sin(fi+particles.at(i).direction)/sin(fi);
        double dxg = dist*cos(travelAngle);
        double dyg = dist*sin(travelAngle);

        particles.at(i).x+=dxg;
        particles.at(i).y+=dyg;
        particles.at(i).addToDirectionAndNormalize(dyaw);
    }
}

void ParticleFilter::calcFitness(double xGps, double yGps)
{
    double distanceSum =0;
    double longestDistance =0;
    for (int i = 0; i < particles.size(); i++) {
        Particle* p = & (particles.at(i));

        p->fitness= sqrt((xGps-p->x)*(xGps-p->x)+(yGps-p->y)*(yGps-p->y));//calc distance
        distanceSum += p->fitness;//store largest distance
        if(p->fitness>longestDistance) longestDistance = p->fitness;
        //if(p->fitness>GPS_DIST_ERR)p->isValid=false;
    }
    double avgDist = distanceSum/particles.size();
    for (int i = 0; i < particles.size(); i++) {
        particles.at(i).fitness = avgDist/particles.at(i).fitness;
    }
    //todo calc amount of fitness for one descendant, iterate trough particles, if fitnes > min add new particle, decrease fit by amount

}
void ParticleFilter::calcFitness(double xGps, double yGps, double gpsErr)
{
    //   gpsErr /=ParticleFilter::radiOfEarthForDegr;// convert to gps degrees
    gpsErr =gpsErr*2/ParticleFilter::radiOfEarthForDegr;// convert to gps degrees
    double distanceSum =0;
    double longestDistance =0;
    int validCount =0;
    for (int i = 0; i < particles.size(); i++) {
        Particle* p = & (particles.at(i));

        p->fitness= sqrt((xGps-p->x)*(xGps-p->x)+(yGps-p->y)*(yGps-p->y));//calc distance
        if(p->fitness>gpsErr)p->isValid=false;
        else{
            validCount++;
            distanceSum += p->fitness;//store largest distance
            if(p->fitness>longestDistance) longestDistance = p->fitness;
        }
    }
    //    reduceUnequality(0.1,longestDistance);// reduce difference between weigths to include more particles in regen

    distanceSum =0;
    for (int i = 0; i < particles.size(); i++) {
        if(particles.at(i).isValid)
            distanceSum+=longestDistance - particles.at(i).fitness;
    }

    //  double avgDist = distanceSum/validCount;//todo calc amount of desc
    for (int i = 0; i < particles.size(); i++) {
        particles.at(i).fitness = 4*PARTICLE_COUNT*(longestDistance-particles.at(i).fitness)/distanceSum;
    }
    //todo calc amount of fitness for one descendant, iterate trough particles, if fitnes > min add new particle, decrease fit by amount
    notValidCount = particles.size()-validCount;
}
void ParticleFilter::calcFitnessGeneric(double xOther, double yOther, double deviation,double measuredDist)
{

    double distanceSum =0;
    double longestDistance =0;
    int validCount =0;
    for (int i = 0; i < particles.size(); i++) {
        Particle* p = & (particles.at(i));

        p->fitness= sqrt((xOther-p->x)*(xOther-p->x)+(yOther-p->y)*(yOther-p->y));//calc distance
        p->fitness = std::abs(p->fitness-measuredDist);
        if(p->fitness>deviation)p->isValid=false;
        else{
            validCount++;
            distanceSum += p->fitness;//store largest distance
            if(p->fitness>longestDistance) longestDistance = p->fitness;
        }
    }
    //    reduceUnequality(0.1,longestDistance);// reduce difference between weigths to include more particles in regen

    distanceSum =0;
    for (int i = 0; i < particles.size(); i++) {
        if(particles.at(i).isValid)
            distanceSum+=longestDistance - particles.at(i).fitness;
    }

    //  double avgDist = distanceSum/validCount;//todo calc amount of desc
    for (int i = 0; i < particles.size(); i++) {
        particles.at(i).fitness = 1*PARTICLE_COUNT*(longestDistance-particles.at(i).fitness)/distanceSum;
    }
    //todo calc amount of fitness for one descendant, iterate trough particles, if fitnes > min add new particle, decrease fit by amount
    notValidCount = particles.size()-validCount;
}
void ParticleFilter::calcFitnessFromYaw(double yawGPS)
{
    double distanceSum =0;
    double longestDistance =0;
    for (int i = 0; i < particles.size(); i++) {
        Particle* p = & (particles.at(i));

        p->fitness= std::abs(yawGPS-p->direction);
        distanceSum += p->fitness;//store largest distance
        if(p->fitness>longestDistance) longestDistance = p->fitness;
        //if(p->fitness>GPS_DIST_ERR)p->isValid=false;
    }
    double avgDist = distanceSum/particles.size();
    for (int i = 0; i < particles.size(); i++) {
        particles.at(i).fitness = avgDist/particles.at(i).fitness;
    }
    //todo calc amount of fitness for one descendant, iterate trough particles, if fitnes > min add new particle, decrease fit by amount

}
void ParticleFilter::regenerateParticles()
{
    std::cout<<"particle count: "<<particles.size()<<std::endl;
    std::sort(particles.begin(), particles.end());// sort by fitness, at this point fitness should point, how much descendants particle should have
    std::vector<Particle> particlesRegenerated;
    int parentCount =0;
    for (int i = particles.size()-1; i >= 0; i--) {
        if(!particles.at(i).isValid) continue;
        int descendantCount =   (int)(particles.at(i).fitness+0.5);// round up
        parentCount ++;
        //create descendants
        for (int j = 0; j < descendantCount; ++j) {
            particlesRegenerated.push_back(particles.at(i));//todo distribute descendants spatially
            if(particlesRegenerated.size()>=PARTICLE_COUNT)break;

        }
        if(particlesRegenerated.size()>=PARTICLE_COUNT)break;

    }
    std::cout<<"nr of parents "<<parentCount<<" max descendants count: "<<(((particles.at(particles.size()-1).fitness))+0.5)<<" notValidCount: "<<notValidCount <<std::endl;

    particles = particlesRegenerated;// should we copy data  or adress only?

}

void ParticleFilter::addMovementNoise()
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,0.02);// stddev value?
    std::normal_distribution<double> angledistribution(0.0,M_PI/128);// stddev value?

    for (int i = 0; i < particles.size(); i++) {



        double errx = (distribution(generator));
        double erry = (distribution(generator));
        double errYaw = (angledistribution(generator));

        particles.at(i).x+=errx;//remove only, because wheel slip cant produce larger distance than measured by odo?
        particles.at(i).y+=erry;
        particles.at(i).addToDirectionAndNormalize(errYaw);

    }
}
void ParticleFilter::addRegenNoise()
{
    for (int i = 0; i < particles.size(); i++) {

        double errx = (regenSpatialDist(generator));
        double erry = (regenSpatialDist(generator));


        particles.at(i).x+=errx;
        particles.at(i).y+=erry;

    }
}

void ParticleFilter::reduceUnequality(double coef, double maxWeigth)// c: 0...1 , large coef means eaqual weigths, small - unchanged
{
    for (int i = 0; i < particles.size(); i++) {

        particles.at(i).fitness = particles.at(i).fitness/(maxWeigth-(maxWeigth-particles.at(i).fitness)*coef);
    }
}

void ParticleFilter::addLinearMovementNoise(double dt)// for testing wo actual odometry from wheels - asuuume that linear speed can be from 0 to 1 m/s
{
    //  std::default_random_engine generator;
    // std::normal_distribution<double> distribution(1.0,1.0);// stddev value? //mean = 1m/s

    for (int i = 0; i < particles.size(); i++) {
        double dist = dt*(linMovementDistribution(generator));
        dist = dist/ParticleFilter::radiOfEarthForDegr; //converting from m to degrees, because lat, lon is degrees
        // particles.at(i).moveForward(std::abs(dist));
        particles.at(i).moveForward(dist);// testing w moving both directions

    }
}
Particle ParticleFilter::calcAverageParticle()
{
    Particle avg(0,0,0);
    //    avg.direction=0;
    //    avg.x=0;
    //    avg.y=0;
    double maxDeltaYawSoFar = 0;// find max dist of two consecutive particles, it should represent deviation TODO- improve to exact method
    double sina =0;// for cirlular mean
    double cosa =0;
    for (int i = 0; i < particles.size(); i++) {

        sina += std::sin(particles.at(i).direction);
        cosa += std::cos(particles.at(i).direction);

        avg.x+=particles.at(i).x;
        avg.y+=particles.at(i).y;
        if(i>0){
            double dYaw =  std::abs(particles.at(i).direction-particles.at(i-1).direction);
            if(dYaw > maxDeltaYawSoFar)maxDeltaYawSoFar = dYaw;

        }
    }
    sina /=particles.size();
    cosa /=particles.size();

    avg.direction =  std::atan2(sina,cosa);
    //avg.direction = std::remainder(avg.direction,2*M_PI);// converting to -pi..+pi
    avg.x =  avg.x/particles.size();
    avg.y =  avg.y/particles.size();
    deltaYaw = maxDeltaYawSoFar*180/M_PI;
    avgParticle = avg;
    return avg;
}



void ParticleFilter::initializeParticles(double x, double y) {
    particles.clear();
    for (int i = 0; i < PARTICLE_COUNT; i++) {


        particles.push_back( Particle(x, y, (rand() % 360)*M_PI / 180 ));
        // particles.push_back( Particle(x, y, 0));
    }
    addRegenNoise();// to move particles away from single point, as in this case fitness function will be zero

}

//for initializing particles at measured distance from beacon in all directions
void ParticleFilter::initializeParticles(double bx, double by, double dist) {
    initializeParticles(bx,by); // particles at beacon position in all directions
    moveForward(dist);
    //randomize direction after movement, because now all particles is facing away from beacon


    for (int i = 0; i < particles.size(); i++) {

        particles.at(i).direction = (rand() % 360)*M_PI / 180 ;
    }
    addRegenNoise();// to move particles away from single point, as in this case fitness function will be zero

}


Particle::Particle(double x, double y, double yaw)
{
    this->x=x;
    this->y = y;
    this->direction = yaw;
}
