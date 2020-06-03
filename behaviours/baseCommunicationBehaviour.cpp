#include "baseCommunicationBehaviour.hpp"
#include "roombaAgent.hpp"
#include <algorithm>
#include "roombaMovementManager.hpp"
BaseCommunicationBehaviour::BaseCommunicationBehaviour(RoombaAgent* roombaAgent)
{
    owner = roombaAgent;
}

VSMMessage* BaseCommunicationBehaviour::receive(MessageContents messageContents)// neeeded?
{//find msg with required content
  if(msgDeque.empty())return 0;

    std::deque<VSMMessage>::iterator it = msgDeque.begin();

     while (it != msgDeque.end()){
     if((*it).contentDescription==messageContents){
     VSMMessage* res = new VSMMessage(&(*it));
     msgDeque.erase(it);
     return res;
     }

         it++;
     }


      // msgDeque.pop_front();
return 0;
}

void BaseCommunicationBehaviour::subscribeToTopic(Topics topic)
{
    owner->subscribersMap.at((int)topic).insert(this);
}

void BaseCommunicationBehaviour::subscribeToDirectMsgs()
{
    owner->subscribersMap.at(owner->id).insert(this);

}

void BaseCommunicationBehaviour::unSubscribeToTopic(Topics topic)
{
    std:set<BaseCommunicationBehaviour*> subs = owner->subscribersMap.at((int)topic);
    subs.erase(this);// remove by value
}

void BaseCommunicationBehaviour::behaviourStep()
{
    //
}

void BaseCommunicationBehaviour::remove()
{
owner->movementManager->stopMotion();//stop robot when removing behaviour
    owner->removeBehaviour(this);
}

void BaseCommunicationBehaviour::logKeypoints(std::string text)// for debuging
{
  //std::cout<<text;
}
