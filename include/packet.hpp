#ifndef PACKET_HPP
#define PACKET_HPP

#include "udp_packet.hpp"

class Packet : public Udp_Packet
{
public:
    Packet() : Udp_Packet() {}
    Packet(const QByteArray& theSource);

protected:
    virtual QByteArray payload() const { return payld; }

private:
    QByteArray payld;
    unsigned int payloadStart;
};

#endif // PACKET_HPP
