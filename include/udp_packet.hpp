#ifndef UDP_PACKET_HPP
#define UDP_PACKET_HPP

#include "base_packet.hpp"

class Udp_Packet : public Base_Packet
{
public:
    Udp_Packet() : Base_Packet() {}
    Udp_Packet(const QByteArray& theSource);

    virtual quint16 sourceUdpPort() const final;
    virtual quint16 destinationUdpPort() const final;

protected:
    static constexpr unsigned int UDP_HEADER_SIZE = 8;
    static constexpr unsigned int UDP_SRCPORT_POS = 0;
    static constexpr unsigned int UDP_PORT_SIZE = 2;
    static constexpr unsigned int UDP_DSTPORT_POS = UDP_SRCPORT_POS + UDP_PORT_SIZE;

    QByteArray header() const { return Base_Packet::header() + udpHeader; }
    virtual unsigned int udpHeaderStartPos() const final { return udpHeaderStart; }
    virtual unsigned int udpHeaderSize() const final { return udpHeader.size(); }

private:
    QByteArray udpHeader;
    unsigned int udpHeaderStart;

    quint16 getPort(const unsigned int thePos) const;
};

#endif // UDP_PACKET_HPP
