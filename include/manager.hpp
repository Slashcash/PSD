#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QObject>
#include <QVector>
#include <QScopedPointer>
#include <QTimer>
#include <QPair>
#include <QTimer>

#include "interface.hpp"
#include "base_packet.hpp"

class Manager : public QObject
{
Q_OBJECT

public:
    struct InjectOperation
    {
        QHostAddress ip;
        unsigned int netmask;
        Interface::InterfaceType interfaceType;
        QString interfaceName;
        QByteArray pokemon;
    };

    Manager(QObject* parent = nullptr);
    Manager(const Manager& theManager) = delete;

    Manager& operator=(const Manager& theManager) = delete;

    bool inject(const InjectOperation& theOperation);
    void stopInjection();

signals:
    void injectionStarted(const InjectOperation& theOperation);
    void injectionStopped();
    void sessionKeyChanged(const QHostAddress& theAddress, const QByteArray& theKey);
    void tradeStarted(const QHostAddress& theAddress);
    void tradeIntercepted(const QHostAddress& theAddress);
    void tradeStopped();
    void tradeInterrupted();

private slots:
    void onMsgReceived(const QByteArray& theMsg);

private:
    static constexpr int tradeKeepAlive = 2000;

    void startScan(const Interface::InterfaceType& theType, const QString& theName);
    void stopScan();
    void stopTrade();
    QHostAddress broadcast() const;
    Base_Packet* handleBreplyPacket(const QByteArray& theMsg);
    Base_Packet* handleNormalPacket(const QByteArray& theMsg);
    Base_Packet* handlePiaPacket(const QByteArray& theMsg);

    QScopedPointer<Interface> interface_ptr;
    QMetaObject::Connection interfaceConnection;
    QHash<const QHostAddress, QByteArray> keys;
    QByteArray injectionPokemon;
    QHostAddress injectionIp;
    unsigned int injectionNetmask;
    bool scanning;
    bool injecting;
    QHostAddress activeTrade;
    QTimer tradeTimer;
    QByteArray receivedPokemon;
};

#endif // MANAGER_HPP
