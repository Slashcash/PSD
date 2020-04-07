#include "interface.hpp"

#include "log.hpp"

Interface::Interface(const QString& theName, QObject* parent) : QObject(parent)
{
    interfaceName = theName;

    connect(this, &Interface::msgReceivedEffective, this, &Interface::onMsgReceived);
    connect(this, &Interface::operationStarted, this, &Interface::onOperationStarted);
    connect(this, &Interface::operationEnded, this, &Interface::onOperationEnded);

    qCInfo(interfac) << "Starting interface initialization";
}

bool Interface::send(const QByteArray theMsg)
{
    qCInfo(interfac) << "Sending" << theMsg.size() << "bytes to" << name();

    operationStarted(OperationType::SEND);

    bool ok = sendEffective(theMsg);
    if(!ok) qCCritical(interfac) << "Sending to" << name() << "failed";

    operationEnded(OperationType::SEND, ok);

    return ok;
}

void Interface::onMsgReceived(const QByteArray& theMsg)
{
    qCInfo(interfac) << "Received" << theMsg.size() << "bytes from" << name();
    emit msgReceived(theMsg);
}

void Interface::onOperationStarted(const OperationType& theOperationType) const
{
    switch(theOperationType)
    {
        case OperationType::SEND:
        {
            emit sendOperationStarted();
        }
        break;
        default:
        {
            qCWarning(interfac) << "Unhandled operation type started";
        }
        break;
    }
}

void Interface::onOperationEnded(const OperationType& theOperationType, const bool theResult) const
{
    switch(theOperationType)
    {
        case OperationType::SEND:
        {
            emit sendOperationEnded(theResult);
        }
        break;
        default:
        {
            qCWarning(interfac) << "Unhandled operation type ended";
        }
        break;
    }
}
