#ifndef BASECOMMUNICATIONBEHAVIOUR_HPP
#define BASECOMMUNICATIONBEHAVIOUR_HPP

//#include <deque>
#include "uwbmsglistener.hpp"
//#include "roombaAgent.hpp"
class RoombaAgent;


class BaseCommunicationBehaviour{

public:
    BaseCommunicationBehaviour(RoombaAgent *roombaAgent);
RoombaAgent* owner;
     std::deque<VSMMessage> msgDeque;

void subscribeToTopic(Topics topic);
void unSubscribeToTopic(Topics topic);
virtual void behaviourStep();
virtual void remove();

};




#endif //S3BEHAVIOUR_HPP
