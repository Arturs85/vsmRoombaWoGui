#include "baseCommunicationBehaviour.hpp"
#include "roombaAgent.hpp"
#include <algorithm>
BaseCommunicationBehaviour::BaseCommunicationBehaviour(RoombaAgent* roombaAgent)
{
    owner = roombaAgent;
}

void BaseCommunicationBehaviour::subscribeToTopic(Topics topic)
{
    owner->subscribersMap.at(topic).push_back(this);
}

void BaseCommunicationBehaviour::unSubscribeToTopic(Topics topic)
{
    vector<BaseCommunicationBehaviour*> subs = owner->subscribersMap.at(topic);
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
