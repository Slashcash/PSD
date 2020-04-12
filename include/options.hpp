#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <QString>
#include <QCommandLineParser>

extern const QString APP_NAME;
extern const QString APP_DESCTRIPTION;
extern const QString APP_VERSION;

extern QCommandLineParser parser;
extern QCommandLineOption addressOption;
extern QCommandLineOption typeOption;
extern QCommandLineOption nameOption;
extern QCommandLineOption fileOption;

int parseArguments(QCoreApplication& app);

#endif // OPTIONS_HPP
