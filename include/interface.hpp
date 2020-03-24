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
        INITIALIZE = 0,
        SEND,
    };

    Interface(const Interface& theInterface) = delete;
    Interface(QObject* parent = nullptr);

    virtual bool send(const QByteArray theMsg) final;
    virtual QString name() const = 0;

    Interface& operator=(const Interface& theInterface) = delete;

signals:
    virtual void msgReceived(const QByteArray& theMsg) const final;
    virtual void operationStarted(const OperationType& theOperationType) const final;
    virtual void operationEnded(const OperationType& theOperationType, const bool theResult) const final;
    virtual void initializeOperationStarted() const final;
    virtual void initializeOperationEnded(const bool theResult) const final;
    virtual void sendOperationStarted() const final;
    virtual void sendOperationEnded(const bool theResult) const final;

    void msgReceivedEffective(const QByteArray& theMsg);

private slots:
    virtual void onMsgReceived(const QByteArray& theMsg) final;
    virtual void onOperationStarted(const OperationType& theOperationType) const final;
    virtual void onOperationEnded(const OperationType& theOperationType, const bool theResult) const final;

private:
    virtual bool sendEffective(const QByteArray& theMsg) = 0;
};

#endif // INTERFACE_HPP
