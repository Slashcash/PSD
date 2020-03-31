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

    Manager manager;
    Manager::InjectOperation op;
    op.interfaceName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    op.interfaceType = Interface::InterfaceType::FILESYSTEM;
    op.ip = QHostAddress("10.13.37.42");
    op.pokemon = QByteArray();
    manager.inject(op);

    app.exec();
}
