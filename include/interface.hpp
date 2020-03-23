#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <QByteArray>
#include <QObject>

class Interface : public QObject
{
Q_OBJECT

public:
    Interface(QObject* parent = nullptr);

    virtual bool send(const QByteArray theMsg) final;
    virtual QString name() const = 0;

signals:
    virtual void msgReceived(const QByteArray& theMsg) final;
    void msgReceivedEffective(const QByteArray& theMsg);

private slots:
    void onMsgReceived(const QByteArray& theMsg);

private:
    virtual bool sendEffective(const QByteArray& theMsg) = 0;
};

#endif // INTERFACE_HPP
