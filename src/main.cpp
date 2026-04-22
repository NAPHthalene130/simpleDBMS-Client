#include "mainwindow.h"
#include "debug/DebugWindow.h"
#include "network/NetworkManager.h"

#include <QApplication>
#include <QLocale>
#include <QObject>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "simpleDBMS-Client_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    DebugWindow debugWindow(&w);
    NetworkManager *networkManager = w.getNetworkManager();
    if (networkManager != nullptr) {
        networkManager->start();
    }

    QObject::connect(&a,
                     &QApplication::aboutToQuit,
                     [networkManager]() {
                         if (networkManager != nullptr) {
                             networkManager->stop();
                         }
                     });

    w.show();
    debugWindow.show();
    return a.exec();
}
