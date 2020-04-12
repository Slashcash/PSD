#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <QByteArray>
#include <QObject>

class Interface : public QObject
{
Q_OBJECT

public:
    enum class OperationType
    {
        SEND = 0
    };

    enum class InterfaceType
    {
        FILESYSTEM = 0,
        PCAP,
        SLP
    };

    Interface(const Interface& theInterface) = delete;
    Interface(const QString& theName, QObject* parent = nullptr);

    virtual bool send(const QByteArray theMsg) final;
    virtual QString name() const final { return interfaceName; }
    virtual InterfaceType type() const = 0;

    Interface& operator=(const Interface& theInterface) = delete;

signals:
    void msgReceived(const QByteArray& theMsg) const;
    void operationStarted(const OperationType& theOperationType) const;
    void operationEnded(const OperationType& theOperationType, const bool theResult) const;
    void sendOperationStarted() const;
    void sendOperationEnded(const bool theResult) const;

    void msgReceivedEffective(const QByteArray& theMsg);

private slots:
    virtual void onMsgReceived(const QByteArray& theMsg) final;
    virtual void onOperationStarted(const OperationType& theOperationType) const final;
    virtual void onOperationEnded(const OperationType& theOperationType, const bool theResult) const final;

private:
    virtual bool sendEffective(const QByteArray& theMsg) = 0;

    QString interfaceName;
};

#endif // INTERFACE_HPP
