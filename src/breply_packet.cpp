#include "breply_packet.hpp"

#include <openssl/hmac.h>
#include <openssl/evp.h>

#include "log.hpp"

const char* Breply_Packet::HMAC_KEY = "p1frXqxmeCZWFv0X";

QByteArray Breply_Packet::obtainSessionKey() const
{
    //initializing ctx
    HMAC_CTX* ctx;
    if( (ctx = HMAC_CTX_new()) == nullptr )
    {
        qCCritical(packetBreply) << "Failed to initialize ctx";
        return QByteArray();
    }

    //setting key
    QByteArray key(HMAC_KEY);
    HMAC_Init_ex(ctx, key.data(), BREPLY_HMACKEY_SIZE, EVP_sha256(), nullptr);


    //updating ctx
    QByteArray sessionParam = modifiedSessionParam();
    if( HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(sessionParam.data()), BREPLY_SESSIONPARAM_SIZE) != 1 )
    {
        qCCritical(packetBreply) << "Failed to update ctx";
        HMAC_CTX_free(ctx);
        return QByteArray();
    }

    unsigned int hmacLength;
    QByteArray bufferKey;
    bufferKey.resize(EVP_MAX_MD_SIZE);
    if( HMAC_Final(ctx, reinterpret_cast<unsigned char*>(bufferKey.data()), &hmacLength) != 1)
    {
        qCCritical(packetBreply) << "Failed to finalize ctx";
        HMAC_CTX_free(ctx);
        return QByteArray();
    }

    QByteArray finalKey;
    for(unsigned int i = 0; i < BREPLY_HMACKEY_SIZE; i++) finalKey.push_back(bufferKey[i]);

    HMAC_CTX_free(ctx);

    return finalKey;
}

QByteArray Breply_Packet::modifiedSessionParam() const
{
    QByteArray buffer = sessionParam();
    buffer[BREPLY_SESSIONPARAM_SIZE-1] = buffer[BREPLY_SESSIONPARAM_SIZE-1] + 1;

    return buffer;
}

