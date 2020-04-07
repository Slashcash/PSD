#include <QCoreApplication>
#include <QStandardPaths>
#include "manager.hpp"
#include "fs_interface.hpp"
#include <QThread>
#include <QLoggingCategory>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    //QLoggingCategory::setFilterRules("interface.info = false");

    const QString appName = "PSD";
    QCoreApplication::setApplicationName(appName);
    QCoreApplication app(argc, argv);

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test1.ek8";
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "WHAT THE FUCK IS THIS SHIT";
    }


    Manager manager;
    Manager::InjectOperation op;
   // op.interfaceName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/triad.pcapng";
    //op.interfaceType = Interface::InterfaceType::PCAP;
    op.interfaceName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    op.interfaceType = Interface::InterfaceType::FILESYSTEM;
    op.ip = QHostAddress("10.13.17.42");
    op.pokemon = file.read(file.size());
    op.netmask = 16;
    manager.inject(op);

    app.exec();
}
