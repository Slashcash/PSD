#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QObject>
#include <QVector>
#include <QScopedPointer>
#include <QTimer>
#include <QPair>

#include "interface.hpp"
#include "base_packet.hpp"

class Manager : public QObject
{
Q_OBJECT

public:
    struct InjectOperation
    {
        QHostAddress ip;
        Interface::InterfaceType interfaceType;
        QString interfaceName;
        QByteArray pokemon;
    };

    Manager(QObject* parent = nullptr);
    Manager(const Manager& theManager) = delete;

    Manager& operator=(const Manager& theManager) = delete;

    bool inject(const InjectOperation& operation);
    void stopInjection();

signals:
    void injectionStarted(const InjectOperation& injection);
    void injectionStopped();
    void sessionKeyChanged(const QHostAddress& address, const QByteArray& key);

private slots:
    void onMsgReceived(const QByteArray& theMsg);

private:
    void startScan(const Interface::InterfaceType& theType, const QString& theName);
    void stopScan();

    Base_Packet* handleBreplyPacket(const QByteArray& theMsg);
    Base_Packet* handleNormalPacket(const QByteArray& theMsg);
    Base_Packet* handlePiaPacket(const QByteArray& theMsg);

    QScopedPointer<Interface> interface;
    QMetaObject::Connection interfaceConnection;
    QHash<const QHostAddress, QByteArray> keys;
    QByteArray injectionPokemon;
    QHostAddress injectionIp;
    bool scanning;
    bool injecting;
};

#endif // MANAGER_HPP
