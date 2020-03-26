#include <QCoreApplication>
#include <QStandardPaths>
#include "manager.hpp"
#include "fs_interface.hpp"
#include <QThread>

int main(int argc, char *argv[])
{
    const QString appName = "PSD";
    QCoreApplication::setApplicationName(appName);
    QCoreApplication app(argc, argv);

    //Manager manager;
    //manager.startScan(Interface::InterfaceType::FILESYSTEM, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    //manager.stopScan();


    FS_Interface interface;
    QByteArray a;
    a.push_back('c');
    interface.send(a);
    QThread::sleep(5);
    interface.send(a);

    app.exec();
}
