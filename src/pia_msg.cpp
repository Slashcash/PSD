#include "pia_msg.hpp"

#include <cstring>
#include <QtEndian>
#include <QStandardPaths>

#include "log.hpp"

Pia_Msg::Pia_Msg() : Data()
{
    parent = nullptr;
}

Pia_Msg::Pia_Msg(const QByteArray& theSource, const Pia_Msg* theParent) : Data()
{
    parent = theParent;

    //discovering pia msgheader_size
    unsigned int headerSize = MSG_FLAG_SIZE;
    bool sizePresent = false;
    unsigned int sizePos = MSG_FLAG_SIZE;

    QByteArray buffer = readFromSource(theSource, MSG_FLAG_POS, MSG_FLAG_SIZE);
    if(buffer.isEmpty()) return;
    char flag = buffer[0];

    if( flag & 1 ) { headerSize += MSG_TYPE_SIZE; sizePos += MSG_TYPE_SIZE; }
    if( flag & 2 ) { headerSize += MSG_SIZE_SIZE; sizePresent = true; }
    if( flag & 4 ) headerSize += MSG_PROT_SIZE;
    if( flag & 8 ) headerSize += MSG_DEST_SIZE;
    if( flag & 16 ) headerSize += MSG_STATION_SIZE;

    quint16 payldSize;
    if(sizePresent)
    {
        QByteArray sizeBuffer = readFromSource(theSource, sizePos, MSG_SIZE_SIZE);
        if(sizeBuffer.isEmpty()) return;
        std::memcpy(&payldSize, sizeBuffer.data(), 2);
        payldSize = qToBigEndian(payldSize);
    }
    else
    {
        if(parent == nullptr) return;
        payldSize = parent->payloadSize();
    }

    unsigned int paddingSize = (headerSize + payldSize) % 4;
    if(paddingSize != 0) paddingSize = 4 - paddingSize;

    QByteArray headerBuffer, payloadBuffer, paddingBuffer;
    if((headerBuffer = readFromSource(theSource, 0, headerSize)).isEmpty()) return;
    if((payloadBuffer = readFromSource(theSource, headerSize, payldSize)).isEmpty()) return;
    if(paddingSize && (paddingBuffer = readFromSource(theSource, headerSize+payldSize, paddingSize)).isEmpty()) return;
    msg_header = headerBuffer;
    payld = payloadBuffer;
    pad = paddingBuffer;
}

int Pia_Msg::containsPokemon() const
{
    quint16 magicNumber;
    QByteArray buffer = readFromSource(payld, MSG_MAGICNUM_POS, MSG_MAGICNUM_SIZE);
    if(buffer.isEmpty()) return false;
    std::memcpy(&magicNumber, buffer.data(), MSG_MAGICNUM_SIZE);
    magicNumber = qToBigEndian(magicNumber);

    if(magicNumber == MSG_MAGICNUM_1) { return MSG_MAGICNUM1_POS; }
    else if(magicNumber == MSG_MAGICNUM_2) { return MSG_MAGICNUM2_POS; }
    else if(magicNumber == MSG_MAGICNUM_ACK) { return MSG_MAGICNUMACK_POS; }
    else if(payld.size() >= 344)
    {
        qCWarning(packetPia) << "A message that may contain a Pokemon has been traded, writing it to file to analyze it further";
        //save();
    }

    return -1;
}

QByteArray Pia_Msg::pokemon() const
{
    int position = containsPokemon();
    if(position == -1) return QByteArray();

    QByteArray buffer = readFromSource(payld, position, POKEMON_SIZE);
    if(buffer.isEmpty()) return QByteArray();
    else return buffer;
}

bool Pia_Msg::inject(const QByteArray& thePokemon)
{
    int position;
    if((position = containsPokemon()) == -1) return false;

    return writeToSource(thePokemon, payld, position);
}
