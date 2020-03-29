#ifndef BREPLY_PACKET_HPP
#define BREPLY_PACKET_HPP

#include "packet.hpp"

class Breply_Packet : public Packet
{
public:
    Breply_Packet() : Packet() {}
    Breply_Packet(const QByteArray& theSource) : Packet(theSource) {}

    QByteArray obtainSessionKey() const;

private:
    static const char* HMAC_KEY;
    static const unsigned int BREPLY_HMACKEY_SIZE = 16;
    static const unsigned int BREPLY_CRYPTOCHALLENGE_SIZE = 58;
    static const unsigned int BREPLY_SESSIONPARAM_SIZE = 32;

    QByteArray sessionParam() const { return readFromSource(payload(), payloadSize() - BREPLY_CRYPTOCHALLENGE_SIZE - BREPLY_SESSIONPARAM_SIZE, BREPLY_SESSIONPARAM_SIZE); }
    QByteArray modifiedSessionParam() const;
};

#endif // BREPLY_PACKET_HPP
