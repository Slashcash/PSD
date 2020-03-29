#include "packet.hpp"

Packet::Packet(const QByteArray& theSource) : Udp_Packet(theSource)
{
    payloadStart = udpHeaderStartPos() + UDP_HEADER_SIZE;

    payld = readFromSource(theSource, payloadStart, theSource.size() - payloadStart);
}
