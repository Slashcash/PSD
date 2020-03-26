#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QObject>
#include <QVector>
#include <QScopedPointer>
#include <QTimer>

#include "interface.hpp"

class Manager : public QObject
{
Q_OBJECT

public:
    Manager(QObject* parent = nullptr);
    Manager(const Manager& theManager) = delete;

    Manager& operator=(const Manager& theManager) = delete;

    void startScan(const Interface::InterfaceType& theType, const QString& theName);
    void stopScan();

private slots:
    void onMsgReceived(const QByteArray& theMsg);

private:
    QScopedPointer<Interface> interface;
    QMetaObject::Connection interfaceConnection;
    bool scanning;

};

#endif // MANAGER_HPP
