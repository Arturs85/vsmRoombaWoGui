#ifndef BASECOMMUNICATIONBEHAVIOUR_HPP
#define BASECOMMUNICATIONBEHAVIOUR_HPP

//#include <deque>
#include "uwbmsglistener.hpp"
//#include "roombaAgent.hpp"

#define TICK_PERIOD_MS 1000

class RoombaAgent;


class BaseCommunicationBehaviour{

public:
    BaseCommunicationBehaviour(RoombaAgent *roombaAgent);
RoombaAgent* owner;
     std::deque<VSMMessage> msgDeque;

VSMMessage* receive(MessageContents messageContents);
void subscribeToTopic(Topics topic);
void subscribeToDirectMsgs();

void unSubscribeToTopic(Topics topic);
virtual void behaviourStep();
virtual void remove();

};




#endif //S3BEHAVIOUR_HPP
