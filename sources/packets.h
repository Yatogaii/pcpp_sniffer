#ifndef  _PACKETS_H_
#define  _PACKETS_H_

#include <string>

#include "protocols.h"

class OriginPacket {
public:
    virtual std::string serialize();
    OriginPacket() = default;
};

class TcpPacket : OriginPacket {

};

#endif
