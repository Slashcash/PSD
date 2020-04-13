#include "slp_interface.hpp"

#include <QDir>

#include "log.hpp"

const QString SLP_Interface::lhost = "127.0.0.1";

SLP_Interface::SLP_Interface(const QString& theServer) : Interface(theServer)
{
    qCInfo(interfaceSlp) << "Opening sockets";
    if(!socket.open(QIODevice::OpenModeFlag::ReadWrite))
    {
        qCCritical(interfaceSlp) << "Socket opening failed:";
        return;
    }

    if(!socket.bind(QHostAddress(lhost), RCV_PORT_NUM))
    {
        qCCritical(interfaceSlp) << "Socket binding failed:";
        return;
    }

    qCInfo(interfaceSlp) << "Opening SLP process";
    slp.start(QDir::currentPath() + "/lan-play.exe", QStringList() << "--relay-server-addr" << theServer);
    if(!slp.waitForStarted())
    {
        qCCritical(interfaceSlp) << "Failed to start SLP";
        return;
    }

    if(!socket.waitForReadyRead(5000))
    {
        qCCritical(interfaceSlp) << "Failed to initialize SLP";
        return;
    }

    QByteArray initBuffer;
    initBuffer.resize(socket.bytesAvailable());
    socket.readDatagram(initBuffer.data(), MAX_PACKET_SIZE);
    if(initBuffer.size() != 1 || initBuffer.at(0) != ACK)
    {
        qCCritical(interfaceSlp) << "Received malformed data after PSD initialization";
        return;
    }



    connect(&socket, &QUdpSocket::readyRead, this, &SLP_Interface::onReadyRead);
}

void SLP_Interface::onReadyRead()
{
    QByteArray buffer;
    buffer.resize(socket.bytesAvailable());
    socket.readDatagram(buffer.data(), MAX_PACKET_SIZE);

    if(buffer.size() != 0) emit msgReceivedEffective(buffer);
}

bool SLP_Interface::sendEffective(const QByteArray& theMsg)
{
    return socket.writeDatagram(theMsg, QHostAddress(lhost), SND_PORT_NUM) == theMsg.size();
}

SLP_Interface::~SLP_Interface()
{
    socket.close();
    slp.close();
    slp.waitForFinished(1000);
}
