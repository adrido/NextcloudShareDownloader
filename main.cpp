#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Adrido");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Nextcloudsync");

    QTranslator qt_translator;
    // look up e.g. :/translations/qt_de.qm
    if (qt_translator.load(QLocale(), QLatin1String("qt"), QLatin1String("_"), QLatin1String("translations/")))
        a.installTranslator(&qt_translator);
    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("NextcloudShareDownloader"), QLatin1String("_"), QLatin1String("translations/")))
        a.installTranslator(&translator);



    MainWindow w;
    w.show();
    return a.exec();
}
