#ifndef BASEPROTOCOL_HPP
#define BASEPROTOCOL_HPP
#include "baseCommunicationBehaviour.hpp"

enum class ProtocolStates{STARTED, FINISHED,WAITING_REPLY,WAITING_START_REQUEST,FIRST_MOVE,FIRST_MOVE_MEASUREMENT_RECEIVED,WAITING_DIST_MEASURE_RESULT,
                          TURN_DEGREES,TIMEOUT,SECOND_MOVE,SECOND_MOVE_MEASUREMENT_RECEIVED,FINAL_POSITION_TURN,FINAL_POSITION_MOVE,FINAL_POSITION_MOVE_MEAS_RECEIVED,
                         WAITING_FORMATION_COMPLETE};
enum class RoleInProtocol{INITIATOR, RESPONDER, SENDER, RECEIVER,STANDING_BEACON,MOVING_BEACON,S2BEACON,S3};


class BaseProtocol{

public:
    BaseProtocol(BaseCommunicationBehaviour* ownerBeh);
RoleInProtocol roleInProtocol;
bool wasSuccessful =false;

protected:
int waitTicksCounter =0;
    ProtocolStates state;
BaseCommunicationBehaviour* behaviour;
static vector<int> stringTointVector(std::string s);
static std::string intVectorToString(vector<int> iv);
private:
virtual bool tick();
virtual void start();



};





#endif //BASEPROTOCOL_HPP
