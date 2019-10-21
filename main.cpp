#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    // look up e.g. :/translations/myapp_de.qm
    if (translator.load(QLocale(), QLatin1String("qt"), QLatin1String("_"), QLatin1String("translations/")))
        a.installTranslator(&translator);

    QCoreApplication::setOrganizationName("Adrido");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Nextcloudsync");

    MainWindow w;
    w.show();
    return a.exec();
}
