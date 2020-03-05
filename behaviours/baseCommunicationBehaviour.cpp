#include "baseCommunicationBehaviour.hpp"
#include "roombaAgent.hpp"
#include <algorithm>
BaseCommunicationBehaviour::BaseCommunicationBehaviour(RoombaAgent* roombaAgent)
{
    owner = roombaAgent;
}

VSMMessage *BaseCommunicationBehaviour::receive(MessageContents messageContents)// neeeded?
{//find msg with required content
  if(msgDeque.empty())return 0;

    std::deque<VSMMessage>::iterator it = msgDeque.begin();

     while (it != msgDeque.end()){
     if((*it).contentDescription==messageContents){
     return &(*it);
     }

         it++;
     }


      // msgDeque.pop_front();
}

void BaseCommunicationBehaviour::subscribeToTopic(Topics topic)
{
    owner->subscribersMap.at((int)topic).push_back(this);
}

void BaseCommunicationBehaviour::subscribeToDirectMsgs()
{
    owner->subscribersMap.at(owner->id).push_back(this);

}

void BaseCommunicationBehaviour::unSubscribeToTopic(Topics topic)
{
    vector<BaseCommunicationBehaviour*> subs = owner->subscribersMap.at((int)topic);
    subs.erase(std::remove(subs.begin(), subs.end(), this), subs.end());// remove by value
}

void BaseCommunicationBehaviour::behaviourStep()
{
    //
}

void BaseCommunicationBehaviour::remove()
{

owner->removeBehaviour(this);
}
