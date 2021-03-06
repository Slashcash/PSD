#include "fs_interface.hpp"

#include <QThread>

#include "log.hpp"

const QString FS_Interface::defaultDirName = "psd_fs_interface";
const QString FS_Interface::defaultSndDirName = "snd";
const QString FS_Interface::defaultRcvDirName = "rcv";

const QString FS_Interface::filePrefix = "packet_";
const QString FS_Interface::fileExtension = ".bin";

FS_Interface::FS_Interface(const QString& theFolderPath) : Interface(theFolderPath)
{
    bool result = true;

    sndDir.setPath(theFolderPath + "/" + defaultDirName + "/" + defaultSndDirName);
    rcvDir.setPath(theFolderPath + "/" + defaultDirName + "/" + defaultRcvDirName);

    result = evaluateDirectoryValidity(sndDir) | evaluateDirectoryValidity(rcvDir);

    sndDir.setFilter(QDir::Filter::Files | QDir::Filter::Readable | QDir::Filter::NoDotAndDotDot);
    sndDir.setNameFilters(QStringList() << "*" + fileExtension);
    sndDir.setSorting(QDir::SortFlag::Name);

    rcvDir.setFilter(QDir::Filter::Files | QDir::Filter::Readable | QDir::Filter::NoDotAndDotDot);
    rcvDir.setSorting(QDir::SortFlag::Name);

    if(result)
    {
        connect(&timer, &QTimer::timeout, this, &FS_Interface::onTimeout);
        timer.start(pollingTimer);
    }
}

bool FS_Interface::evaluateDirectoryValidity(const QDir &theDir) const
{
    if(!theDir.exists())
    {
        qCWarning(interfaceFs) << "The directory does not exist, creating it in:" << theDir.absolutePath();
        if(!theDir.mkpath("."))
        {
            qCCritical(interfaceFs) << "Directory creation failed at" << theDir.absolutePath();
            return false;
        }
    }

    if(!theDir.isReadable())
    {
        qCCritical(interfaceFs) << "Directory" << theDir.absolutePath() << "is not readable";
        return false;
    }

    return true;
}

bool FS_Interface::sendEffective(const QByteArray& theMsg)
{
    unsigned int fileNumber;

    sndDir.refresh();
    QStringList fileList = sndDir.entryList();

    if(fileList.empty()) fileNumber = 1;

    else
    {
        QString lastFileName = fileList.last();

        int numberPosition;
        if((numberPosition = lastFileName.indexOf(filePrefix)) == -1)
        {
            qCCritical(interfaceFs) << "Found a malformed file name (" << lastFileName << ")";
            return false;
        }

        bool ok;
        fileNumber = lastFileName.mid(numberPosition+filePrefix.size(), lastFileName.size() - filePrefix.size() - fileExtension.size()).toInt(&ok) + 1;
        if(!ok)
        {
            qCCritical(interfaceFs) << "Found a malformed file name (" << lastFileName << ")";
            return false;
        }
    }

    QString filePath = sndDir.absolutePath() + '/' + filePrefix + QString::number(fileNumber) + fileExtension;
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly))
    {
        qCCritical(interfaceFs) << "Failed to open file" << filePath;
        return false;
    }

    qint64 writtenSize = file.write(theMsg);
    if(writtenSize != theMsg.size() || writtenSize == -1)
    {
        qCCritical(interfaceFs) << "Error in writing" << filePath;
        file.close();
        return false;
    }

    file.close();
    return true;
}

void FS_Interface::onTimeout()
{
    rcvDir.refresh();
    QStringList newFileList = rcvDir.entryList();
    for(auto it : newFileList)
    {
        if(!fileCache.contains(it))
        {
            QString filePath = rcvDir.absolutePath() + '/' + it;
            QFile file(filePath);
            if(!file.open(QIODevice::ReadOnly))
            {
                qCCritical(interfaceFs) << "Failed to open file" << filePath;
            }
            else
            {
                emit msgReceivedEffective(file.read(file.size()));
            }
        }
    }

    fileCache = newFileList;
}
