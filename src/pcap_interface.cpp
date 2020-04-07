#include "pcap_interface.hpp"

#include <QStandardPaths>

#include "log.hpp"

const QString PCAP_Interface::defaultDirName = "psd_pcap_interface";
const QString PCAP_Interface::defaultSndDirName = "snd";
const QString PCAP_Interface::filePrefix = "packet_";
const QString PCAP_Interface::fileExtension = ".bin";


PCAP_Interface::PCAP_Interface(const QString& theFilePath) : Interface(theFilePath)
{
    dump = nullptr;
    fp = nullptr;
    sndDir.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultDirName + "/" + defaultSndDirName);

    if(!evaluateDirectoryValidity(sndDir)) return;

    char errbuf[PCAP_ERRBUF_SIZE];
    char source[PCAP_BUF_SIZE];

    if (pcap_createsrcstr(source, PCAP_SRC_FILE, NULL,	NULL, theFilePath.toStdString().c_str(), errbuf) != 0)
    {
        qCCritical(interfacePcap) << "Failed to create source string";
        return;
    }

    if ((fp = pcap_open(source, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf) ) == NULL)
    {
        qCCritical(interfacePcap) << "Failed to open file" << theFilePath;
        return;
    }

    dump = pcap_dump_open(fp, (sndDir.absolutePath() + "/out.pcapng").toStdString().c_str());

    if(dump == NULL)
    {
        qCCritical(interfacePcap()) << "Failed to open output file";
        return;
    }

    waitTimer.setInterval(waitTime);
    waitTimer.setSingleShot(true);
    waitTimer.callOnTimeout([this](){pcap_loop(fp, 0, &PCAP_Interface::dispatch, reinterpret_cast<u_char*>(this));});
    waitTimer.start();
}

void PCAP_Interface::dispatch(u_char* temp1, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
    QByteArray buffer;
    for(unsigned int i = 0; i < header->caplen; i++)
    {
        buffer.push_back(pkt_data[i]);
    }

    emit reinterpret_cast<PCAP_Interface*>(temp1)->msgReceivedEffective(buffer);
}

bool PCAP_Interface::sendEffective(const QByteArray& theMsg)
{
    struct pcap_pkthdr pktHeader;
    struct timeval ts;
    pktHeader.len = theMsg.size();
    pktHeader.caplen = theMsg.size();
    pktHeader.ts = ts;
    pcap_dump(reinterpret_cast<unsigned char*>(dump), &pktHeader, reinterpret_cast<const unsigned char*>(theMsg.data()));
    return true;
}

bool PCAP_Interface::evaluateDirectoryValidity(const QDir &theDir) const
{
    if(!theDir.exists())
    {
        qCWarning(interfacePcap) << "The directory does not exist, creating it in:" << theDir.absolutePath();
        if(!theDir.mkpath("."))
        {
            qCCritical(interfacePcap) << "Directory creation failed at" << theDir.absolutePath();
            return false;
        }
    }

    if(!theDir.isReadable())
    {
        qCCritical(interfacePcap) << "Directory" << theDir.absolutePath() << "is not readable";
        return false;
    }

    return true;
}

PCAP_Interface::~PCAP_Interface()
{
    if(dump != nullptr) pcap_dump_close(dump);
    if(fp != nullptr) pcap_close(fp);
}
