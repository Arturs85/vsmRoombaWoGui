#ifndef BASEPROTOCOL_HPP
#define BASEPROTOCOL_HPP
#include "baseCommunicationBehaviour.hpp"

enum class ProtocolStates{STARTED, FINISHED,WAITING_REPLY,WAITING_START_REQUEST,FIRST_MOVE,FIRST_MOVE_MEASUREMENT_RECEIVED,WAITING_DIST_MEASURE_RESULT,
                          TURN_DEGREES,TIMEOUT,SECOND_MOVE,SECOND_MOVE_MEASUREMENT_RECEIVED,FINAL_POSITION_TURN,FINAL_POSITION_MOVE,FINAL_POSITION_MOVE_MEAS_RECEIVED,
                          WAITING_FORMATION_COMPLETE,IDLE,WAITING_CONFIRM_ROLE,FIRST_MEASUREMENT_RECEIVED_FROM_1,FIRST_MEASUREMENT_RECEIVED_FROM_2,
                          FIRST_MOVE_MEASUREMENT_RECEIVED_FROM_2, SECOND_MOVE_MEASUREMENT_RECEIVED_FROM_2, FINAL_POSITION_MOVE_MEAS_RECEIVED_FROM_2,ACKNOWLEDGE_RECEIVED,
                          WAITING_ACKNOWLEDGE,MASTER_MEAS_RECEIVED,B1_MEAS_RECEIVED,B2_MEAS_RECEIVED,BEACONS_DEPLOYED, QUERRY_WAITING_REPLY};
enum class RoleInProtocol{INITIATOR, RESPONDER, SENDER, RECEIVER,STANDING_BEACON,MOVING_BEACON,S2BEACON,S3,BEACON,BEACON_MASTER,CLIENT,
                          S2EXPLORERS,EXPLORER,S2BASE, S4};


class BaseProtocol{

public:
    BaseProtocol(BaseCommunicationBehaviour* ownerBeh);
    RoleInProtocol roleInProtocol;
    bool wasSuccessful =false;
    static vector<int> stringTointVector(std::string s);
    static std::string intVectorToString(vector<int> iv);

protected:
    int waitTicksCounter =0;
    ProtocolStates state;
    ProtocolStates onPositiveQuerry;
    ProtocolStates onNoReply;
    VSMMessage* receivedReplyOnQuerry;
 VSMMessage querryMessage;
 MessageContents expectedReplyContents;
    void querryWithTimeout(VSMMessage message,MessageContents replyContents, ProtocolStates onReply, ProtocolStates noReply, int retries, int timeout);
    int  querrieTicksCounter=0;
    int querrieRetryCounter=0;
    int querieTicksMax =3;
    int querryRetriesMax =3;//defaults
    BaseCommunicationBehaviour* behaviour;

private:
    virtual bool tick();
    virtual void start();



};





#endif //BASEPROTOCOL_HPP
