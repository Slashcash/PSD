#ifndef BREPLY_PACKET_HPP
#define BREPLY_PACKET_HPP

#include "udp_packet.hpp"

class Breply_Packet : public Udp_Packet
{
public:
    Breply_Packet() : Udp_Packet() {}
    Breply_Packet(const QByteArray& theSource);

    QByteArray obtainSessionKey() const;

protected:
    virtual QByteArray payload() const { return payld; }

private:
    static const char* HMAC_KEY;
    static const unsigned int BREPLY_HMACKEY_SIZE = 16;
    static const unsigned int BREPLY_CRYPTOCHALLENGE_SIZE = 58;
    static const unsigned int BREPLY_SESSIONPARAM_SIZE = 32;

    QByteArray sessionParam() const { return readFromSource(payld, payloadSize() - BREPLY_CRYPTOCHALLENGE_SIZE - BREPLY_SESSIONPARAM_SIZE, BREPLY_SESSIONPARAM_SIZE); }
    QByteArray modifiedSessionParam() const;

    QByteArray payld;
    unsigned int payloadStart;
};

#endif // BREPLY_PACKET_HPP
