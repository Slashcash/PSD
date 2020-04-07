#ifndef PCAP_INTERFACE_HPP
#define PCAP_INTERFACE_HPP

#include <pcap.h>
#include <QDir>
#include <QTimer>

#include "interface.hpp"

class PCAP_Interface : public Interface
{
Q_OBJECT

public:
    explicit PCAP_Interface(const QString& theFilePath);
    virtual InterfaceType type() const { return InterfaceType::PCAP; }

    ~PCAP_Interface();

private:
    static constexpr int waitTime = 1000;

    static const QString defaultDirName;
    static const QString defaultSndDirName;
    static const QString filePrefix;
    static const QString fileExtension;

    static void dispatch(u_char* temp1, const struct pcap_pkthdr* header, const u_char* pkt_data);
    virtual bool sendEffective(const QByteArray& theMsg);
    bool evaluateDirectoryValidity(const QDir& theDir) const;

    QDir sndDir;
    QTimer waitTimer;
    pcap_dumper_t* dump;
    pcap_t* fp;
};

#endif // PCAP_INTERFACE_HPP
