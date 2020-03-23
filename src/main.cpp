#include <QCoreApplication>
#include "fs_interface.hpp"

int main(int argc, char *argv[])
{
    const QString appName = "PSD";
    QCoreApplication::setApplicationName(appName);
    QCoreApplication app(argc, argv);

    FS_Interface test;
    app.exec();
}
