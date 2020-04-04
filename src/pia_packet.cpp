#include "pia_packet.hpp"

#include <cstring>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <QFile>
#include <QtEndian>

#include "log.hpp"

Pia_Packet::Pia_Packet() : Udp_Packet()
{
    decrypted = false;
}

Pia_Packet::Pia_Packet(const QByteArray& theSource) : Udp_Packet(theSource)
{
    decrypted = false;

    piaHeaderStart = udpHeaderStartPos() + UDP_HEADER_SIZE;
    payloadStart = piaHeaderStart + PIA_HEADER_SIZE;

    piaHeader = readFromSource(theSource, piaHeaderStart, PIA_HEADER_SIZE);
    encPayload = readFromSource(theSource, payloadStart, theSource.size() - payloadStart);
}

Pia_Packet::Pia_Packet(const QByteArray& theSource, const QByteArray& theKey)
{
    piaHeaderStart = udpHeaderStartPos() + UDP_HEADER_SIZE;
    payloadStart = piaHeaderStart + PIA_HEADER_SIZE;
    piaHeader = readFromSource(theSource, piaHeaderStart, PIA_HEADER_SIZE);
    encPayload = readFromSource(theSource, payloadStart, theSource.size() - payloadStart);
    setKey(theKey);
}

void Pia_Packet::setKey(const QByteArray& theKey)
{
    key = theKey;
    QByteArray decryptedSource = decrypt();
    if(decryptedSource.isEmpty()) decrypted = false;
    else
    {
        Pia_Msg* parent = nullptr;
        char tempBuffer = decryptedSource[0];
        for(int i = 0; i < decryptedSource.size();  i += messages.last().size())
        {
            QByteArray buffer = readFromSource(decryptedSource, i, decryptedSource.size() - i);
            if(buffer.isEmpty()) return;
            messages.push_back(Pia_Msg(buffer, parent));
            parent = &messages.last();
            int newMsgPos = i + messages.last().size();
            if(newMsgPos == 0)
            {
                qCCritical(packetPia) << "packet payload seems to be malformed";
                return;
            }

            if(newMsgPos == decryptedSource.size()) break;

            tempBuffer = decryptedSource[newMsgPos];
            if(*reinterpret_cast<unsigned char*>(&tempBuffer) == 0xFF)
            {
                QByteArray padBuffer = readFromSource(decryptedSource, newMsgPos, decryptedSource.size() - newMsgPos);
                if(padBuffer.isEmpty()) return;
                pad = padBuffer;
                break;
            }
        }

        decrypted = true;
    }
}

unsigned int Pia_Packet::connectionId() const
{
    QByteArray buffer = readFromSource(piaHeader, PIA_CONNECTIONID_POS, PIA_CONNECTIONID_SIZE);
    if(buffer.isEmpty()) return 0;
    else return buffer[0];
}

QByteArray Pia_Packet::decrypt() const
{
    EVP_CIPHER_CTX* ctx;
    //Create and initialise the context
    if( !(ctx = EVP_CIPHER_CTX_new()) )
    {
        qCCritical(crypto) << "Failed to initialize the context";
        return QByteArray();
    }

    //Initialise the decryption operation
    if( !EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, nullptr, nullptr))
    {
        qCCritical(crypto) << "Failed to initialize decryption operation";
        return QByteArray();
    }

    //Initialise key and IV
    QByteArray bufferIv = iv();
    if( !EVP_DecryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(bufferIv.data())))
    {
        qCCritical(crypto) << "Failed to initialize iv";
        return QByteArray();
    }

    //provide message
    unsigned char* plaintext = new unsigned char[encPayload.size()];
    int plaintext_length;
    if( !EVP_DecryptUpdate(ctx, plaintext, &plaintext_length, reinterpret_cast<const unsigned char*>(encPayload.data()), encPayload.size()) )
    {
        delete [] plaintext;
        qCCritical(crypto) << "Failed to get plaintext";
        return QByteArray();
    }

    //set expected tag value
    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, authTag().data()))
    {
        delete [] plaintext;
        qCCritical(crypto) << "Failed to set the authentication tag";
        return QByteArray();
    }

    //finalize the decryption
    if( EVP_DecryptFinal_ex(ctx, plaintext, &plaintext_length) != 1 )
    {
        delete [] plaintext;
        qCCritical(crypto) << "Failed to finalize decryption";
        return QByteArray();
    }

    EVP_CIPHER_CTX_free(ctx);

    QByteArray result;
    for(int i = 0; i < encPayload.size(); i++) result.push_back(plaintext[i]);

    delete [] plaintext;
    return result;
}

QByteArray Pia_Packet::iv() const
{
    QByteArray iv;
    quint32 ip = qToBigEndian(sourceIpAddress().toIPv4Address());
    for(unsigned int i = 0; i < sizeof(quint32); i++)
    {
        char buffer;
        std::memcpy(&buffer, reinterpret_cast<char*>(&ip)+i, 1);
        iv.append(buffer);
    }

    iv.append(connectionId());
    iv.append(ivPart());

    return iv;
}

QByteArray Pia_Packet::decryptedPayload() const
{
    QByteArray payloadBuffer;
    for(int i = 0; i < messages.size(); i++) payloadBuffer += messages[i].rawData();
    return payloadBuffer + padding();
}
