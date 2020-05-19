#include "s1ExchangeProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing
#include <unistd.h>
#include "s3Behaviour.hpp"
#include "s2BaseBehaviour.hpp"
#include "baseS1ManagementProtocol.hpp"

//for use in s2 behaviours, sends requests to other s2 and waits for answers
S1ExchangeProtocol::S1ExchangeProtocol( BaseCommunicationBehaviour *ownerBeh):BaseProtocol(ownerBeh)
{

    behaviour->subscribeToTopic(Topics::S1_REQUESTS);
}

void S1ExchangeProtocol::start()
{

}

bool S1ExchangeProtocol::tick()
{
    VSMMessage* res = behaviour->receive(MessageContents::REQUEST_S1);
    if(res!= 0){// check if there is robot to give and perform it if possible
        int askersCVal = std::stoi(res->content);
        if(askersCVal>((S2BaseBehavior*)behaviour)->lastControlValue-((S2BaseBehavior*)behaviour)->lastS1Count){
            //give agent, except if this is s2beacons with no more than 3 beacons
        }
        int beaconId =((S2BaseBehavior*)behaviour)->getS1IdForGiveaway();// VSMMessage reply(behaviour->owner->id,Topics::TWO_POINT_FORMATION_TO_MOVING,MessageContents::AGREE,"a");
        if(beaconId!=0){
            //send message to s1 to change type
            // res->content
            VSMSubsystems typeToChangeTo;
            if(res->sender==VSMSubsystems::S2_EXPLORERS) typeToChangeTo = VSMSubsystems::S1_EXPLORERS;
            else if(res->sender==VSMSubsystems::S2_BEACONS) typeToChangeTo = VSMSubsystems::S1_BEACONS;

            ((S2BaseBehavior*)behaviour)->s1ManagementProtocol->sendChangeType(beaconId,std::to_string((int)typeToChangeTo));
        }
        // behaviour->owner->sendMsg(reply);

    }
    return false;
}

void S1ExchangeProtocol::askS1(int cValue)
{
    VSMMessage request(behaviour->type,Topics::S1_REQUESTS,MessageContents::REQUEST_S1,std::to_string(cValue));

    behaviour->owner->sendMsg(request);
}


