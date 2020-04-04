#ifndef PIA_PACKET_HPP
#define PIA_PACKET_HPP

#include "udp_packet.hpp"

#include "pia_msg.hpp"

class Pia_Packet : public Udp_Packet
{
public:
    Pia_Packet();
    Pia_Packet(const QByteArray& theSource);
    Pia_Packet(const QByteArray& theSource, const QByteArray& theKey);

    unsigned int connectionId() const;
    void setKey(const QByteArray& theKey);
    QByteArray rawDecryptedData() const { return header() + decryptedPayload(); }
    bool writeDecryptedToFile(const QString& thePath) const { return writeSourceToFile(rawDecryptedData(), thePath); }

protected:
    virtual unsigned int piaHeaderStartPos() const final { return piaHeaderStart; }
    QByteArray header() const { return Udp_Packet::header() + piaHeader; }
    virtual QByteArray payload() const { return encPayload; }

private:
    static constexpr unsigned int PIA_HEADER_SIZE = 32;
    static constexpr unsigned int PIA_CONNECTIONID_SIZE = 1;
    static constexpr unsigned int PIA_CONNECTIONID_POS = 5;
    static constexpr unsigned int PIA_AUTHTAG_POS = 16;
    static constexpr unsigned int PIA_AUTHTAG_SIZE = 16;
    static constexpr unsigned int PIA_IVPART_POS = 9;
    static constexpr unsigned int PIA_IVPART_SIZE = 7;

    QByteArray authTag() const { return readFromSource(piaHeader, PIA_AUTHTAG_POS, PIA_AUTHTAG_SIZE); }
    QByteArray ivPart() const { return readFromSource(piaHeader, PIA_IVPART_POS, PIA_IVPART_SIZE); }
    QByteArray iv() const;
    QByteArray decrypt() const;
    QByteArray encrypt() const;
    QByteArray padding() const { return pad; }
    QByteArray decryptedPayload() const;

    QByteArray piaHeader;
    QByteArray encPayload;
    QByteArray key;
    QVector<Pia_Msg> messages;
    QByteArray pad;
    bool decrypted;
    unsigned int piaHeaderStart;
    unsigned int payloadStart;
};

#endif // PIA_PACKET_HPP
