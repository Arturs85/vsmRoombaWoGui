#ifndef ROLECHECKINGPROTOCOL_HPP
#define ROLECHECKINGPROTOCOL_HPP
#include "baseProtocol.hpp"
//enum ProtocolStates{STARTED, FINISHED,WAITING_REPLY};

class RoleCheckingProtocol: public BaseProtocol {

   
   virtual bool tick();
virtual void start();



};





#endif //ROLECHECKINGPROTOCOL_HPP
