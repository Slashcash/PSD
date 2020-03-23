#ifndef FS_INTERFACE_HPP
#define FS_INTERFACE_HPP

#include <QDir>
#include <QStandardPaths>
#include <QTimer>

#include "interface.hpp"

class FS_Interface : public Interface
{
Q_OBJECT

public:
    explicit FS_Interface(const QDir& theSndDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultDirName + "/" + defaultSndDirName,
                           const QDir& theRcvDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultDirName + "/" + defaultRcvDirName);

    QString name() const;

private slots:
    void onTimeout();

private:
    static const QString defaultDirName;
    static const QString defaultSndDirName;
    static const QString defaultRcvDirName;

    static const QString filePrefix;
    static const QString fileExtension;

    static constexpr int pollingTimer = 300;

    QDir rcvDir;
    QDir sndDir;
    QTimer timer;
    QStringList fileCache;

    bool evaluateDirectoryValidity(const QDir& theDir) const;
    virtual bool sendEffective(const QByteArray& theMsg);
};
#endif // FS_INTERFACE_HPP
