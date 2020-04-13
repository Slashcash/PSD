#include <QCoreApplication>
#include <QLoggingCategory>

#include "options.hpp"
#include "log.hpp"
#include "manager.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    //parsing command line arguments
    int parseError;
    if((parseError = parseArguments(app)) != 0) return parseError;

    //disabling verbous log
    QLoggingCategory::setFilterRules("interface.info = false");

    //enabling/disabling suspicious packet log
    if(parser.isSet(saveOption)) saveSuspicious = true;
    else saveSuspicious = false;

    //opening injection file
    QString filePath = parser.value(fileOption);
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCCritical(parsing) << "Failed to open injection file";
        return 2;
    }

    //setting up operation
    Manager manager;
    Manager::InjectOperation op;

    op.interfaceName = parser.value(nameOption);

    QString interfaceType = parser.value(typeOption);
    if(interfaceType == "slp") op.interfaceType = Interface::InterfaceType::SLP;
    else if(interfaceType == "fs") op.interfaceType = Interface::InterfaceType::FILESYSTEM;
    else if(interfaceType == "pcap") op.interfaceType = Interface::InterfaceType::PCAP;
    else op.interfaceType = Interface::InterfaceType::SLP;

    op.ip = QHostAddress(parser.value(addressOption));

    op.pokemon = file.readAll();

    op.netmask = 16;

    //starting injection
    manager.inject(op);

    //starting app execution
    app.exec();
}
