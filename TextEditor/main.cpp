#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("BananaSharePoint-USI");
    QCoreApplication::setOrganizationName("Banana.ch & USI");
    QCoreApplication::setApplicationVersion(
            QString("%1.%2.%3").arg(BA_VERSION_MAJOR).arg(BA_VERSION_MINOR).arg(BA_VERSION_PATCH));
    QCommandLineParser parser;
    parser.setApplicationDescription("R&D about integrating a Qt App with Sharepoint");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "path or url to the file to open.");
    parser.process(app);


    MainWindow mainWin;
    const QStringList posArgs = parser.positionalArguments();
    for (const QString &fileName : posArgs)
        mainWin.openFile(fileName);
    mainWin.show();
    return app.exec();
}
