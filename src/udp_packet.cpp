#include "udp_packet.hpp"

#include <QtEndian>
#include <cstring>

Udp_Packet::Udp_Packet(const QByteArray& theSource) : Base_Packet(theSource)
{
    udpHeaderStart = IP_START_POS + ipHeaderSize();

    udpHeader = readFromSource(theSource, udpHeaderStart, UDP_HEADER_SIZE);
}

quint16 Udp_Packet::sourceUdpPort() const
{
    return getPort(UDP_SRCPORT_POS);
}

quint16 Udp_Packet::destinationUdpPort() const
{
    return getPort(UDP_DSTPORT_POS);
}

quint16 Udp_Packet::getPort(const unsigned int thePos) const
{
    QByteArray portArray = readFromSource(udpHeader, thePos, UDP_PORT_SIZE);
    if(portArray.isEmpty()) return 0;

    quint16 port;
    std::memcpy(&port, portArray.data(), UDP_PORT_SIZE);
    port = qToBigEndian(port);

    return port;
}
