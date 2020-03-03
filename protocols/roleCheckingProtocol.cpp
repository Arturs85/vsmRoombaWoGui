#include "roleCheckingProtocol.hpp"
#include "vSMMessage.hpp"
#include "roombaAgent.hpp"// for editing


bool RoleCheckingProtocol::tick()
{

    switch (state) {
    case ProtocolStates::STARTED:

            break;
    default:
        break;
    }

}

void RoleCheckingProtocol::start()
{
 state = STARTED;
 //send message to s3 asking for role,
 VSMMessage request(S1,Topics::S3_,RoleCheckWithS3,"roleCh");
behaviour->owner->uwbMsgListener.addToTxDeque(request);// null ptr check
 state = WAITING_REPLY;
}
