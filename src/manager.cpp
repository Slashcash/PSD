#include "manager.hpp"

#include <cmath>

#include "fs_interface.hpp"
#include "pcap_interface.hpp"
#include "log.hpp"
#include "packet.hpp"
#include "breply_packet.hpp"
#include "pia_packet.hpp"
#include "slp_interface.hpp"

Manager::Manager(QObject* parent) : QObject(parent)
{
    scanning = false;
    injecting = false;
    injectionNetmask = 0;
    tradeTimer.callOnTimeout([this](){
        qCWarning(manager) << "Timeout reached for the trade session with" << activeTrade.toString() <<"stopping it now";
        emit tradeInterrupted();
        Manager::stopTrade();
    });

    tradeTimer.setInterval(tradeKeepAlive);
}

bool Manager::inject(const InjectOperation& theOperation)
{
    if(injecting)
    {
        qCWarning(manager) << "An injection operation is already in progress, stopping it before starting a new one";
        stopInjection();
    }

    if(theOperation.pokemon.size() != Pia_Msg::POKEMON_SIZE)
    {
        qCCritical(manager) << "Pokemon size is incorrect, impossible to inject";
        return false;
    }

    if(theOperation.ip.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
    {
        qCCritical(manager) << "IP seems to be not valid";
        return false;
    }

    if(theOperation.netmask > 32)
    {
        qCCritical(manager) << "Netmask seems to be not valid";
        return false;
    }

    qCInfo(manager) << "Starting a new injection operation for" << theOperation.ip.toString();
    injecting = true;

    injectionPokemon = theOperation.pokemon;
    injectionIp = theOperation.ip;
    injectionNetmask = theOperation.netmask;

    startScan(theOperation.interfaceType, theOperation.interfaceName);
    emit injectionStarted({injectionIp, injectionNetmask, interface_ptr.data()->type(), interface_ptr.data()->name(), injectionPokemon});
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
        injectionNetmask = 0;
        activeTrade = QHostAddress();
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

    interface_ptr->send(msgPtr->rawData());
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
    Pia_Packet* buffer = new Pia_Packet(theMsg);
    if(buffer->destinationIpAddress() == injectionIp || buffer->sourceIpAddress() == injectionIp)
    {
        unsigned int packetConnId = buffer->connectionId();

        QHostAddress ip;
        if(buffer->sourceIpAddress() == injectionIp) { ip = buffer->destinationIpAddress(); }
        else ip = buffer->sourceIpAddress();

        auto it = keys.find(ip);
        QByteArray key;
        it == keys.end() ? key = QByteArray() : key = it.value();

        if(packetConnId == 0)
        {
            if(ip == activeTrade) stopTrade();
        }

        else
        {
            if(ip != activeTrade && ip != broadcast())
            {
                activeTrade = ip;
                emit tradeStarted(ip);
                if(key.isEmpty()) { qCWarning(manager) << "Trade with" << ip.toString() << "started but key has not been intercepted"; }
                else { qCInfo(manager) << "Trade with" << ip.toString() << "started"; emit tradeIntercepted(ip); }
            }

            if(!key.isEmpty())
            {
                buffer->setKey(key);
                if(buffer->containsPokemon() && buffer->destinationIpAddress() == injectionIp)
                {
                    qCInfo(manager) << "A packet containing the Pokemon has been intercepted";
                    receivedPokemon = buffer->pokemon();
                    buffer->inject(injectionPokemon);
                }

                if(buffer->containsAck() && buffer->sourceIpAddress() == injectionIp)
                {
                    qCInfo(manager) << "A packet containing an ACK has been intercepted";
                    if(receivedPokemon.isEmpty()) qCWarning(manager) << "ACK intercepted but we have no Pokemon to inject in, trade will probably crash";
                    else buffer->inject(receivedPokemon);
                    //buffer->saveDecrypted();
                }
            }

            else qCWarning(manager) << "A trade session packet has been received but its key has not been intercepted";

            if(ip == activeTrade) tradeTimer.start();
        }
    }

    return buffer;
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

    qCInfo(manager) << "Starting a new scan operation on" << theName;

    switch(theType)
    {
        case Interface::InterfaceType::FILESYSTEM:
        {
            interface_ptr.reset(new FS_Interface(theName));
        }
        break;
        case Interface::InterfaceType::PCAP:
        {
            interface_ptr.reset(new PCAP_Interface(theName));
        }
        break;
        case Interface::InterfaceType::SLP:
        {
            interface_ptr.reset(new SLP_Interface(theName));
        }
    break;
        default:
        {
            qCWarning(manager) <<  "Unhandled interface type, reverting back to the standard interface";
            interface_ptr.reset(new SLP_Interface(theName));
        }
        break;
    }

    interfaceConnection = connect(interface_ptr.data(), &Interface::msgReceived, this, &Manager::onMsgReceived);
    scanning = true;
}

void Manager::stopScan()
{
    if(!scanning)
    {
        qCWarning(manager) << "Scan stopping requested but no scan is in progress";
    }
    else
    {
        qCInfo(manager) << "Stopping scan operation on" << interface_ptr->name();
        disconnect(interfaceConnection);
        interface_ptr.reset(nullptr);
        scanning = false;
    }
}

QHostAddress Manager::broadcast() const
{
    auto buffer = QHostAddress::parseSubnet(injectionIp.toString()+"/"+QString::number(injectionNetmask));
    return QHostAddress(buffer.first.toIPv4Address() | (static_cast<quint32>(pow(2, buffer.second))-1));
}

void Manager::stopTrade()
{
    qCInfo(manager) << "Trade with" << activeTrade.toString() << "stopped";
    activeTrade = QHostAddress();
    tradeTimer.stop();
    emit tradeStopped();
}
