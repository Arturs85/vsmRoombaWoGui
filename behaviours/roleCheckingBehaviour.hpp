#ifndef ROLECHECKINGBEHAVIOUR_HPP
#define ROLECHECKINGBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"
#include "roleCheckingProtocol.hpp"

#define testInOtherClass 99

class RoleCheckingBehaviour : public BaseCommunicationBehaviour{

public:
RoleCheckingBehaviour(RoombaAgent* owner);
    void behaviourStep();


private:
RoleCheckingProtocol* roleCheckingProtocol;
    void askS3ForRoles(); //send msg to S3 and wait for reply, if no reply, than take s3 role, else take role that s3 sent

};






#endif //ROLECHECKINGBEHAVIOUR_HPP
