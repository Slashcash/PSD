#ifndef PIA_MSG_HPP
#define PIA_MSG_HPP

#include <QByteArray>

#include "data.hpp"

class Pia_Msg : public Data
{
public:
    Pia_Msg();
    Pia_Msg(const QByteArray& theSource, const Pia_Msg* const theParent = nullptr);

    virtual unsigned int size() const { return headerSize() + payloadSize() + paddingSize(); }
    virtual QByteArray rawData() const final { return header() + payload() + padding(); }

 private:
    static constexpr unsigned int MSG_FLAG_POS = 0;
    static constexpr unsigned int MSG_FLAG_SIZE = 1;
    static constexpr unsigned int MSG_TYPE_SIZE = 1;
    static constexpr unsigned int MSG_SIZE_SIZE = 2;
    static constexpr unsigned int MSG_PROT_SIZE = 4;
    static constexpr unsigned int MSG_DEST_SIZE = 8;
    static constexpr unsigned int MSG_STATION_SIZE = 8;

    QByteArray header() const { return msg_header; }
    unsigned int headerSize() const { return header().size(); };
    QByteArray payload() const { return payld; }
    unsigned int payloadSize() const { return payload().size(); }
    QByteArray padding() const { return pad; }
    unsigned int paddingSize() const { return padding().size(); }

    QByteArray msg_header;
    QByteArray payld;
    QByteArray pad;
    const Pia_Msg* parent;
};

#endif // PIA_MSG_HPP
