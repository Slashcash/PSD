#include "manager.hpp"

#include "fs_interface.hpp"
#include "log.hpp"
#include "udp_packet.hpp"

Manager::Manager(QObject* parent) : QObject(parent)
{
    scanning = false;
}

void Manager::onMsgReceived(const QByteArray& theMsg)
{
    interface->send(theMsg);
}

void Manager::startScan(const Interface::InterfaceType &theType, const QString &theName)
{
    if(scanning)
    {
        qCWarning(manager) << "A scan operation is already in progress, stopping it before starting a new one";
        stopScan();
    }

    scanning = true;
    qCInfo(manager) << "Starting a new scan operation on" << theName;

    switch(theType)
    {
        case Interface::InterfaceType::FILESYSTEM:
        {
            interface.reset(new FS_Interface(theName));
        }
        break;
        default:
        {
            qCWarning(manager) <<  "Unhandled interface type";
        }
        break;
    }

    interfaceConnection = connect(interface.data(), &Interface::msgReceived, this, &Manager::onMsgReceived);
}

void Manager::stopScan()
{
    if(!scanning)
    {
        qCWarning(manager) << "Scan stopping requested but no scan is in progress";
    }
    else
    {
        qCInfo(manager) << "Stopping scan operation on" << interface->name();
        disconnect(interfaceConnection);
        interface.reset(nullptr);
        scanning = false;
    }
}
