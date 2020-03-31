#include "manager.hpp"

#include "fs_interface.hpp"
#include "log.hpp"
#include "packet.hpp"
#include "breply_packet.hpp"

Manager::Manager(QObject* parent) : QObject(parent)
{
    scanning = false;
    injecting = false;
}

bool Manager::inject(const InjectOperation& operation)
{
    if(injecting)
    {
        qCWarning(manager) << "An injection operation is already in progress, stopping it before starting a new one";
        stopInjection();
    }

    //TODO: Add a check on pokemon bytearray size and return false if it does not match

    qCInfo(manager) << "Starting a new injection operation for" << operation.ip.toString();
    injecting = true;

    injectionPokemon = operation.pokemon;
    injectionIp = operation.ip;

    startScan(operation.interfaceType, operation.interfaceName);
    emit injectionStarted({injectionIp, interface.data()->type(), interface.data()->name(), injectionPokemon});
    return true;
}

void Manager::stopInjection()
{
    if(!injecting)
    {
        qCWarning(manager) << "Injection stopping requested but no injection is in progress";
    }
    else
    {
        qCInfo(manager) << "Stopping injection operation on" << injectionIp.toString();
        keys.clear();
        injectionPokemon = QByteArray();
        injectionIp = QHostAddress();
        injecting = false;
        stopScan();
        emit injectionStopped();
    }
}

void Manager::onMsgReceived(const QByteArray& theMsg)
{
    Packet buffer(theMsg);
    QScopedPointer<Base_Packet> msgPtr;
    switch(buffer.evaluateType())
    {
        case Base_Packet::Type::NORMAL:
        {
            msgPtr.reset(handleNormalPacket(theMsg));
        }
        break;
        case Base_Packet::Type::BREPLY:
        {
            msgPtr.reset(handleBreplyPacket(theMsg));
        }
        break;
        case Base_Packet::Type::PIA:
        {
            msgPtr.reset(handlePiaPacket(theMsg));
        }
        break;
        default:
        {
            msgPtr.reset(handleNormalPacket(theMsg));
        }
        break;
    }

    interface->send(msgPtr.get()->rawData());
}

Base_Packet* Manager::handleBreplyPacket(const QByteArray& theMsg)
{
    Breply_Packet* buffer = new Breply_Packet(theMsg);

    if(buffer->sourceIpAddress() == injectionIp || buffer->destinationIpAddress() == injectionIp)
    {
        QHostAddress ip;
        if(buffer->sourceIpAddress() == injectionIp) { ip = buffer->destinationIpAddress(); }
        else ip = buffer->sourceIpAddress();

        QByteArray key = buffer->obtainSessionKey();

        auto it = keys.find(ip);
        if(it == keys.end() || it.value() != key)
        {
            qCInfo(manager) << "New session key exchanged with" << ip.toString();
            emit sessionKeyChanged(ip, key);
        }

        keys.insert(ip, key);
    }

    return buffer;
}

Base_Packet* Manager::handlePiaPacket(const QByteArray& theMsg)
{
    return new Packet(theMsg);
}

Base_Packet* Manager::handleNormalPacket(const QByteArray& theMsg)
{
    return new Packet(theMsg);
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
