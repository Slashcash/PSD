#include "interface.hpp"

#include "log.hpp"

Interface::Interface(QObject* parent) : QObject(parent)
{
    connect(this, &Interface::msgReceivedEffective, this, &Interface::onMsgReceived);
    connect(this, &Interface::operationStarted, this, &Interface::onOperationStarted);
    connect(this, &Interface::operationEnded, this, &Interface::onOperationEnded);

    emit operationStarted(OperationType::INITIALIZE);
}

bool Interface::send(const QByteArray theMsg)
{
    qCInfo(interface) << "Sending" << theMsg.size() << "bytes to" << name();

    operationStarted(OperationType::SEND);

    bool ok = sendEffective(theMsg);
    if(!ok) qCCritical(interface) << "Sending to" << name() << "failed";

    operationEnded(OperationType::SEND, ok);

    return ok;
}

void Interface::onMsgReceived(const QByteArray& theMsg)
{
    qCInfo(interface) << "Received" << theMsg.size() << "bytes from" << name();
    emit msgReceived(theMsg);
}

void Interface::onOperationStarted(const OperationType& theOperationType) const
{
    switch(theOperationType)
    {
        case OperationType::INITIALIZE:
        {
            emit initializeOperationStarted();
        }
        break;
        case OperationType::SEND:
        {
            emit sendOperationStarted();
        }
        break;
        default:
        {
            qCWarning(interface) << "Unhandled operation type started";
        }
    }
}

void Interface::onOperationEnded(const OperationType& theOperationType, const bool theResult) const
{
    switch(theOperationType)
    {
        case OperationType::INITIALIZE:
        {
            emit initializeOperationEnded(theResult);
        }
        break;
        case OperationType::SEND:
        {
            emit sendOperationEnded(theResult);
        }
        break;
        default:
        {
            qCWarning(interface) << "Unhandled operation type ended";
        }
    }
}
