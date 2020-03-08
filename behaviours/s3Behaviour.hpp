#ifndef S3BEHAVIOUR_HPP
#define S3BEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
class RoleCheckingProtocol;

class S3Behaviour : public BaseCommunicationBehaviour {
public:
    S3Behaviour(RoombaAgent* owner);

private:
    RoleCheckingProtocol* roleCheckingProtocol;
    void behaviourStep();

};

#endif //S3BEHAVIOUR_HPP
