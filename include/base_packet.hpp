#ifndef BASE_PACKET_HPP
#define BASE_PACKET_HPP

#include <QByteArray>
#include <QHostAddress>

class Base_Packet
{
public:
    Base_Packet();
    Base_Packet(const QByteArray& theSource);

    virtual QHostAddress sourceIpAddress() const final;
    virtual QHostAddress destinationIpAddress() const final;
    virtual QByteArray sourceMacAddress() const final;
    virtual QByteArray destinationMacAddress() const final;

private:
    static constexpr unsigned int ETH_START_POS = 0;
    static constexpr unsigned int ETH_HEADER_SIZE = 14;
    static constexpr unsigned int IP_START_POS = ETH_START_POS + ETH_HEADER_SIZE;
    static constexpr unsigned int IP_SIZE_POS = IP_START_POS;
    static constexpr unsigned int IP_SIZE_SIZE = 1;
    static constexpr unsigned int IP_MIN_SIZE = 20;

    static constexpr unsigned int MAC_ADDRESS_SIZE = 6;
    static constexpr unsigned int MAC_DST_POS = 0;
    static constexpr unsigned int MAC_SRC_POS = MAC_DST_POS + MAC_ADDRESS_SIZE;
    static constexpr unsigned int IP_ADDRESS_SIZE = 4;
    static constexpr unsigned int IP_SRC_POS = 12;
    static constexpr unsigned int IP_DST_POS = IP_SRC_POS + IP_ADDRESS_SIZE;


    virtual QByteArray readFromSource(const QByteArray& theSource, const unsigned int theStartPos, const unsigned int theSize) const final;
    virtual QHostAddress ipAddress(const unsigned int thePos) const final;

    QByteArray ethHeader;
    QByteArray ipHeader;
};

#endif // BASE_PACKET_HPP
