#include "explorerListenerBehaviour.hpp"
#include "localisationProtocolpf.hpp"
#include "explorerManagementProtocol.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>
#include "twoPointFormationProtocol.hpp" //for PI

// add this behaviour when unit becomes of explore type, so it can listen to commands from s2

#define NEXT_TARGET_DISTANCE 2000 // 2m
//#define NEXT_TARGET_DISTANCE 2000 // 2m

#define AREA_RADI 4000

ExplorerListenerBehaviour::ExplorerListenerBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    explorerManagementProtocol  = new ExplorerManagementProtocol(RoleInProtocol::EXPLORER,this);
    localisationProtocol=new LocalisationProtocolPf(RoleInProtocol::CLIENT,this);
    type = VSMSubsystems::S1_EXPLORERS;
//for testing
    startExploring();
}

void ExplorerListenerBehaviour::startExploring()
{
    std::cout<<"elb explorer starting explore\n";
    explorerState=ExplorerStates::IDLE;
    localise(ExplorerStates::FIRST_MEASUREMENT);//for testing
//localise(ExplorerStates::IDLE);//for loop localise

}

void ExplorerListenerBehaviour::stopExploring()
{
    explorerState=ExplorerStates::IDLE;//todo improve
    std::cout<<"elb explorer stopping explore\n";

}

PointInt ExplorerListenerBehaviour::getRandomPointAtDistance(int distance)
{
    // choose x randomly within the radius
    int x = (rand()%(distance*2))-distance;
    int usqr = distance*distance- x*x;
    if(usqr<0) usqr =0; //avois negative values under sqrt
    float y =std::sqrt(usqr);
    PointInt res{x,(int)y};
    return  res;
}


void ExplorerListenerBehaviour::behaviourStep()
{
    BaseCommunicationBehaviour::logKeypoints("emp tick called\n");
    explorerManagementProtocol->tick();
    BaseCommunicationBehaviour::logKeypoints("emp tick returned\n");
    //manage driving

std::cout<<" elb msg size "<<msgDeque.size()<<"\n";

    switch (explorerState) {
    case ExplorerStates::IDLE:
//localise(ExplorerStates::IDLE);//for loop localise

        break;

    case ExplorerStates::FIRST_MEASUREMENT:{// at this point location result should be valid
        previousLocation.x = localisationProtocol->result.at(0);
        previousLocation.y = localisationProtocol->result.at(1);

        odometryBeforeDriving = owner->movementManager->odometry;
        owner->movementManager->driveDistance(NEXT_TARGET_DISTANCE);
        explorerState = ExplorerStates::FIRST_DRIVE;

    }
        break;


    case ExplorerStates::FIRST_DRIVE:{
        if(owner->movementManager->state == MovementStates::FINISHED){//finished moving, localise and pick new dest

            if(owner->movementManager->isObstacleDetected){// this flag is cleared when calling manager.move
                std::cout<<"elb obstacle was detected fd\n";
                //for now just pick next dest point as if no obstacle was detected
            }

            explorerState = ExplorerStates::ARRIVED_DEST;
        }

    }break;


    case ExplorerStates::MEASURING_XY:{
        bool finished = localisationProtocol->tick();
        if(finished){//
            if(localisationProtocol->wasSuccessful)
            {// xy are available, use them for mapping or direction calculation, if this is second measurement after traveling dist
                explorerState= stateAfterLocalise;// return to desired state

            }else {
                localise(stateAfterLocalise);//try localise indefinetly

            }
        }
    }
        break;

    case ExplorerStates::MOVING_FORWARD:{
        if(owner->movementManager->state == MovementStates::FINISHED){//finished moving, localise and pick new dest

            if(owner->movementManager->isObstacleDetected){// this flag is cleared when calling manager.move
                std::cout<<"elb obstacle was detected\n";
                //for now just pick next dest point as if no obstacle was detected
            }

            explorerState = ExplorerStates::ARRIVED_DEST;
        }

    }break;

    case ExplorerStates::ARRIVED_DEST:{

        localise(ExplorerStates::ARRIVED_DEST_AND_LOCALISED);

    }break;

    case ExplorerStates::ARRIVED_DEST_AND_LOCALISED:{

        //compare 2x triang and odom

        int odometryAfterDriving = owner->movementManager->odometry;
        int distanceOdo = odometryAfterDriving-odometryBeforeDriving;

        latestLocation.x = localisationProtocol->result.at(0);
        latestLocation.y = localisationProtocol->result.at(1);
        int dx =latestLocation.x-previousLocation.x;
        int dy =latestLocation.y-previousLocation.y;

        latestDirection = std::atan2(dx,dy);
        int distTriang = std::sqrt(dx*dx+dy*dy);
        std::cout<< "elb distOdo "<<distanceOdo<< " distTriang "<<distTriang<<"\n";

        previousLocation.x = localisationProtocol->result.at(0);
        previousLocation.y = localisationProtocol->result.at(1);

        // pick a new destination
        PointInt dest;
        int absX;
        int absY;
        //check if dest is within coverage
        do{
            dest =getRandomPointAtDistance(NEXT_TARGET_DISTANCE);
            absX = latestLocation.x+dest.x;
            absY = latestLocation.y+dest.y;
            //c
        }while(absX*absX+absY*absY > AREA_RADI*AREA_RADI );
        //turn to new destination direction
        float destAngle = std::atan2(dest.y,dest.x);
        float angleToTurn = destAngle - latestDirection;
        distToTravelAfterTurning = NEXT_TARGET_DISTANCE;
        owner->movementManager->turn(angleToTurn*180/PI);
       // owner->movementManager->turn(0);

        explorerState = ExplorerStates::TURNING;
    }break;

    case ExplorerStates::TURNING:{

        if(owner->movementManager->state == MovementStates::FINISHED){//finished turning, move forward

            odometryBeforeDriving = owner->movementManager->odometry;
            owner->movementManager->driveDistance(distToTravelAfterTurning);
            explorerState = ExplorerStates::MOVING_FORWARD;
        }
    }   break;


    default:
        break;
    }


}

//add localisation protocol as client
void ExplorerListenerBehaviour::localise(ExplorerStates nextState)
{
    localisationProtocol->localise();
    explorerState = ExplorerStates::MEASURING_XY;
    stateAfterLocalise = nextState;
}
