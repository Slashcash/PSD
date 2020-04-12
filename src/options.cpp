#include "options.hpp"

#include "log.hpp"
#include "QHostAddress"

const QString APP_NAME = "PSD";
const QString APP_DESCTRIPTION = "A MITM attack to Pokemon Sw/Sh LAN Mode";
const QString APP_VERSION = "0.1";

QCommandLineParser parser;

QCommandLineOption addressOption(QStringList() << "a" << "address",
                   QCoreApplication::translate("main", "Specify the Nintendo Switch ip <address>."),
                   QCoreApplication::translate("main", "address"));

QCommandLineOption typeOption(QStringList() << "t" << "type",
                   QCoreApplication::translate("main", "Specify the interface <type>."),
                   QCoreApplication::translate("main", "type"));

QCommandLineOption nameOption(QStringList() << "n" << "name",
                   QCoreApplication::translate("main", "Specify the interface <name>."),
                   QCoreApplication::translate("main", "name"));

QCommandLineOption fileOption(QStringList() << "i" << "input",
                   QCoreApplication::translate("main", "Specify the input <file> path."),
                   QCoreApplication::translate("main", "file"));

int parseArguments(QCoreApplication& app)
{
    parser.setApplicationDescription(APP_DESCTRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(addressOption);

    typeOption.setDefaultValue("slp");
    parser.addOption(typeOption);

    parser.addOption(nameOption);

    parser.addOption(fileOption);

    //processing
    parser.process(app);

    //evaluating
    if(!parser.isSet(nameOption)) { qCCritical(parsing) << "Please specify an interface name"; return 1; }
    if(!parser.isSet(fileOption)) { qCCritical(parsing) << "Please specify an input file to inject"; return 1; }
    if(!parser.isSet(addressOption)) { qCCritical(parsing) << "Please specify the Nintendo Switch ip address"; return 1; }

    QHostAddress ip(parser.value(addressOption));
    if(ip.protocol() != QAbstractSocket::IPv4Protocol) { qCCritical(parsing) << "Failed to parse address as a valid ipv4 address"; return 1; }

    return 0;
}
