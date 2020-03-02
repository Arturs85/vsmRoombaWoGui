#ifndef ROLECHECKINGBEHAVIOUR_HPP
#define ROLECHECKINGBEHAVIOUR_HPP
#include "baseCommunicationBehaviour.hpp"


class RoleCheckingBehaviour : public BaseCommunicationBehaviour{

public:
    void behaviourStep();


private:
    void askS3ForRoles(); //send msg to S3 and wait for reply, if no reply, than take s3 role, else take role that s3 sent

};






#endif //ROLECHECKINGBEHAVIOUR_HPP
