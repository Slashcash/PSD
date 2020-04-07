#include "data.hpp"

#include <QFile>
#include <QStandardPaths>
#include <QDateTime>

#include "log.hpp"

const QString Data::savePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/psd_test";

QByteArray Data::readFromSource(const QByteArray& theSource, const unsigned int theStartPos, const unsigned int theSize) const
{
    if(theSource.size() - theStartPos < theSize)
    {
        qCWarning(data) << "Trying to read from malformed data";
        return QByteArray();
    }

    else return theSource.mid(theStartPos, theSize);
}

bool Data::writeToFile(const QString& thePath) const
{
    return writeSourceToFile(rawData(), thePath);
}

bool Data::writeSourceToFile(const QByteArray& theSource, const QString& thePath) const
{
    qCInfo(data) << "Writing" << theSource.size() << "bytes to" << thePath;
    QFile file(thePath);
    if(!file.open(QIODevice::WriteOnly))
    {
        qCCritical(data) << "Failed to open file" << thePath;
        return false;
    }

    qint64 writtenSize = file.write(theSource);
    if(writtenSize != theSource.size() || writtenSize == -1)
    {
        qCCritical(data) << "Error in writing" << thePath;
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool Data::saveSource(const QByteArray& theSource) const
{
    if(!saveDir.exists())
        if(!saveDir.mkpath("."))
            return false;

    return writeSourceToFile(theSource, saveDir.absolutePath() + "/" + QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd_hh-mm-ss-zzz") + ".bin");
}

bool Data::save() const
{
    return saveSource(rawData());
}
