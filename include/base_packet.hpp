#ifndef BASE_PACKET_HPP
#define BASE_PACKET_HPP

#include <QByteArray>
#include <QHostAddress>

class Base_Packet
{
public:
    enum class Type
    {
        NORMAL,
        BREPLY,
        PIA
    };

    Base_Packet();
    Base_Packet(const QByteArray& theSource);

    Type evaluateType();

    virtual QHostAddress sourceIpAddress() const final;
    virtual QHostAddress destinationIpAddress() const final;
    virtual QByteArray sourceMacAddress() const final;
    virtual QByteArray destinationMacAddress() const final;

    virtual QByteArray rawData() const final { return header() + payload(); }
    virtual bool writeToFile(const QString& thePath) const final;

    virtual ~Base_Packet() {}

protected:
    virtual QByteArray readFromSource(const QByteArray& theSource, const unsigned int theStartPos, const unsigned int theSize) const final;

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

    virtual QByteArray header() const { return ethHeader + ipHeader; }
    virtual unsigned int headerSize() const final { return header().size(); }
    virtual QByteArray payload() const = 0;
    virtual unsigned int payloadSize() const final { return payload().size(); }

    virtual unsigned int ipHeaderSize() const final { return ipHeader.size(); }

private:
    virtual QHostAddress ipAddress(const unsigned int thePos) const final;

    QByteArray ethHeader;
    QByteArray ipHeader;
};

#endif // BASE_PACKET_HPP
