#ifndef BASECOMMUNICATIONBEHAVIOUR_HPP
#define BASECOMMUNICATIONBEHAVIOUR_HPP

//#include <deque>
#include "uwbmsglistener.hpp"
//#include "roombaAgent.hpp"

#define TICK_PERIOD_SEC 1.0
struct PointF{float x; float y;};

class RoombaAgent;


class BaseCommunicationBehaviour{

public:
    BaseCommunicationBehaviour(RoombaAgent *roombaAgent);
    RoombaAgent* owner;
    std::deque<VSMMessage> msgDeque;
    VSMSubsystems type= VSMSubsystems::NONE;
    VSMMessage* receive(MessageContents messageContents);
    void subscribeToTopic(Topics topic);
    void subscribeToDirectMsgs();
void clearMsgs();
    void unSubscribeToTopic(Topics topic);
    virtual void behaviourStep();
    virtual void remove();
    static void logKeypoints(std::string text);

};




#endif //S3BEHAVIOUR_HPP
