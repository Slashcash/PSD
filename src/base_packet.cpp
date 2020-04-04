#include "base_packet.hpp"

#include <cstring>
#include <QtEndian>
#include <QFile>

#include "log.hpp"

Base_Packet::Base_Packet() : Data()
{

}

Base_Packet::Base_Packet(const QByteArray& theSource)
{
    ethHeader = readFromSource(theSource, ETH_START_POS, ETH_HEADER_SIZE);

    unsigned int ipSize;
    QByteArray ipSizeArray = readFromSource(theSource, IP_SIZE_POS, IP_SIZE_SIZE);
    if(!ipSizeArray.isEmpty()) ipSize = (ipSizeArray[0] & 15) * 4;
    else ipSize = 0;

    ipHeader = readFromSource(theSource, IP_START_POS, ipSize);
}

QHostAddress Base_Packet::sourceIpAddress() const
{
    return ipAddress(IP_SRC_POS);
}

QHostAddress Base_Packet::destinationIpAddress() const
{
    return ipAddress(IP_DST_POS);
}

QHostAddress Base_Packet::ipAddress(const unsigned int thePos) const
{
    QByteArray ipv4Array = readFromSource(ipHeader, thePos, IP_ADDRESS_SIZE);
    if(ipv4Array.isEmpty()) return QHostAddress();

    quint32 addr;
    std::memcpy(&addr, ipv4Array.data(), IP_ADDRESS_SIZE);
    addr = qToBigEndian(addr);

    QHostAddress ipv4(addr);
    return ipv4;
}

QByteArray Base_Packet::sourceMacAddress() const
{
    return readFromSource(ethHeader, MAC_SRC_POS, MAC_ADDRESS_SIZE);
}

QByteArray Base_Packet::destinationMacAddress() const
{
    return readFromSource(ethHeader, MAC_DST_POS, MAC_ADDRESS_SIZE);
}

Base_Packet::Type Base_Packet::evaluateType()
{
    QByteArray payld = payload();

    if(payld.at(0) == 1) return Type::BREPLY;
    else if(static_cast<unsigned char>(payld.at(0)) == 0x32) return Type::PIA;
    else return Type::NORMAL;
}
