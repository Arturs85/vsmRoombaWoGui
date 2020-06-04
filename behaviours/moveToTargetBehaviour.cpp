#include "moveToTargetBehaviour.hpp"
#include "localisationProtocol.hpp"
#include "explorerListenerBehaviour.hpp"
#include "roombaMovementManager.hpp"
#include <cmath>
#include "twoPointFormationProtocol.hpp" //for PI
#include "beaconManagementProtocol.hpp"
// add this behaviour when unit becomes of explore type, so it can listen to commands from s2

#define FIRST_DISTANCE 500 // 50cm
#define TARGET_DISTANCE_ALLOWANCE 300 // 50cm

//#define NEXT_TARGET_DISTANCE 2000 // 2m

//#define AREA_RADI 5000

MoveToTargetBehaviour::MoveToTargetBehaviour(RoombaAgent *roombaAgent):BaseCommunicationBehaviour(roombaAgent)
{
    this->ra = roombaAgent;
    localisationProtocol=new LocalisationProtocol(RoleInProtocol::CLIENT,this);
    beaconManagementProtocol = new BeaconManagementProtocol(RoleInProtocol::MOVING_BEACON,this);
}

PointInt MoveToTargetBehaviour::getNextPointToTravel()
{


}

void MoveToTargetBehaviour::startRoute()
{
    //route vector should contain valid route
    destinationPoint = route.at(0);
    localise(ExplorerStates::FIRST_MEASUREMENT);


}

void MoveToTargetBehaviour::setTarget(int x, int y)//to set single point route
{
    std::cout<<"mttb setting target x "<<x<<" y "<<y<<"\n";
    PointInt p = {x,y};
    route.push_back(p);
}





void MoveToTargetBehaviour::behaviourStep()
{
    beaconManagementProtocol->tick();// receives cordinates

    switch (movementState) {
    case ExplorerStates::IDLE:
        //listen for coordinates


        break;

    case ExplorerStates::FIRST_MEASUREMENT:{// at this point location result should be valid
        previousLocation.x = localisationProtocol->result.at(0);
        previousLocation.y = localisationProtocol->result.at(1);

        odometryBeforeDriving = owner->movementManager->odometry;
        owner->movementManager->driveDistance(FIRST_DISTANCE);
        movementState = ExplorerStates::FIRST_DRIVE;

    }
        break;


    case ExplorerStates::FIRST_DRIVE:{
        if(owner->movementManager->state == MovementStates::FINISHED){//finished moving, localise and pick new dest

            if(owner->movementManager->isObstacleDetected){// this flag is cleared when calling manager.move
                std::cout<<"elb obstacle was detected fd\n";
                //for now just pick next dest point as if no obstacle was detected
            }

            movementState = ExplorerStates::ARRIVED_DEST;
        }

    }break;


    case ExplorerStates::MEASURING_XY:{
        bool finished = localisationProtocol->tick();
        if(finished){//
            if(localisationProtocol->wasSuccessful)
            {// xy are available, use them for mapping or direction calculation, if this is second measurement after traveling dist
                movementState= stateAfterLocalise;// return to desired state

            }else {
                localise(stateAfterLocalise);//try localise indefinetly

            }
        }
    }
        break;

    case ExplorerStates::MOVING_FORWARD:{
        if(owner->movementManager->state == MovementStates::FINISHED){//finished moving, localise and pick new dest



            movementState = ExplorerStates::ARRIVED_DEST;
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
        std::cout<< "mttb distOdo "<<distanceOdo<< " distTriang "<<distTriang<<" dir "<<180*latestDirection/PI <<"\n";

        previousLocation.x = localisationProtocol->result.at(0);
        previousLocation.y = localisationProtocol->result.at(1);

        //see if we are at target
        int dxDest= destinationPoint.x-latestLocation.x;
        int dyDest= destinationPoint.y-latestLocation.y;
        std::cout<<"mttb dist To Target dx "<<dxDest<<" dy "<<dyDest<<"\n";

        if(10*sqrt(dxDest*dxDest+dyDest*dyDest)<TARGET_DISTANCE_ALLOWANCE){//we are at dest, end
            movementState= ExplorerStates::FINAL_DESTINATION;//todo notify s2 that target point has been reashed
            std::cout<<"mttb    >>>arrived at destination point<<< \n";
            break;
        }
        if(owner->movementManager->isObstacleDetected){// this flag is cleared when calling manager.move
            std::cout<<"mttb obstacle was detected\n";
            //turn 90 degrees and move 50 cm to possibly deal with obstacle, then proceed to the dest point
            //calculate point of possible obst avoidance and add it to the route list
            int x = cos(latestDirection-PI/2)*FIRST_DISTANCE/10;//50cm
            int y = sin(latestDirection-PI/2)*FIRST_DISTANCE/10;//50cm
            PointInt  p = {latestLocation.x+x,latestLocation.y+y};
            route.push_back(p);
            std::cout<<"mttb added point on route to avoid obstacle x "<<p.x<<" y "<<p.y<<"\n";
        }

        // pick a new destination
        PointInt dest;
        if(route.size()>0){
            dest = route.at(route.size()-1);
            route.pop_back();
        }else{//there are no more points on route, but we are not at the destination, try again to go to the destination
            dest = destinationPoint;

        }
        int dxTarget = dest.x-latestLocation.x;
        int dyTarget = dest.y-latestLocation.y;

        //turn to new destination direction
        float destAngle = std::atan2(dyTarget,dxTarget);
        float angleToTurn = destAngle - latestDirection;
        distToTravelAfterTurning = 10*sqrt(dxTarget*dxTarget+dyTarget*dyTarget);
        angleBeforTurning = owner->movementManager->direction;
        owner->movementManager->turn(angleToTurn*180/PI);
        movementState = ExplorerStates::TURNING;


    }break;

    case ExplorerStates::TURNING:{

        if(owner->movementManager->state == MovementStates::FINISHED){//finished turning, move forward
            latestDirection += PI*(owner->movementManager->direction-angleBeforTurning)/180;// keep track on direction after turning, but is this needed?
            std::cout<<"mttb direction after turning "<<latestDirection<<"\n";
            odometryBeforeDriving = owner->movementManager->odometry;
            owner->movementManager->driveDistance(distToTravelAfterTurning);
            movementState = ExplorerStates::MOVING_FORWARD;
        }
    }   break;


    default:
        break;
    }


}

//add localisation protocol as client
void MoveToTargetBehaviour::localise(ExplorerStates nextState)
{
    localisationProtocol->localise();
    movementState = ExplorerStates::MEASURING_XY;
    stateAfterLocalise = nextState;
}
