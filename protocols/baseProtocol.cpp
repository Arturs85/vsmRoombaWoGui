#include "baseProtocol.hpp"

BaseProtocol::BaseProtocol(BaseCommunicationBehaviour *ownerBeh)
{
    this->behaviour = ownerBeh;
}
