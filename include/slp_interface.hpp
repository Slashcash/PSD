#ifndef SLP_INTERFACE_HPP
#define SLP_INTERFACE_HPP

#include <QUdpSocket>
#include <QProcess>

#include "interface.hpp"

class SLP_Interface : public Interface
{
Q_OBJECT

public:
    explicit SLP_Interface(const QString& theServer);

    InterfaceType type() const { return InterfaceType::SLP; }

    ~SLP_Interface();

private slots:
    void onReadyRead();

private:
    static constexpr unsigned int MAX_PACKET_SIZE = 4000;
    static constexpr quint8 ACK = 0x69;
    static constexpr int RCV_PORT_NUM = 6969;
    static constexpr int SND_PORT_NUM = 6868;
    static const QString lhost;

    virtual bool sendEffective(const QByteArray& theMsg);

    QUdpSocket socket;

    QProcess slp;
};

#endif // SLP_INTERFACE_HPP
