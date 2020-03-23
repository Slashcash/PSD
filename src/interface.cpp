#include "interface.hpp"

#include "log.hpp"

Interface::Interface(QObject* parent) : QObject(parent)
{
    connect(this, &Interface::msgReceivedEffective, this, &Interface::onMsgReceived);
}

bool Interface::send(const QByteArray theMsg)
{
    qCInfo(interface) << "Sending " << theMsg.size() << "bytes to " << name();

    bool ok = sendEffective(theMsg);

    if(!ok) qCCritical(interface) << "Sending to " << name() << " failed";

    return ok;
}

void Interface::onMsgReceived(const QByteArray& theMsg)
{
    qCInfo(interface) << "Received" << theMsg.size() << "bytes from" << name();
    emit msgReceived(theMsg);
}

