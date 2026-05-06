#include "mainwindow.h"

#include <QStackedWidget>

#include "network/NetworkManager.h"
#include "ui/AuthWidget.h"
#include "ui/OpePanelWidget.h"
#include "ui/SettingWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      stackedWidget(new QStackedWidget(this)),
      authWidget(new AuthWidget(this)),
      opePanelWidget(new OpePanelWidget(this)),
      settingWidget(new SettingWidget(this)),
      networkManager(new NetworkManager(this))
{
    resize(1200, 800);
    setCentralWidget(stackedWidget);

    setStyleSheet(QString(
        "QMainWindow {"
        "    background-color: #1E1E1E;"
        "}"
    ));

    switchWidget(authWidget);
}

MainWindow::~MainWindow()
{
    delete networkManager;
    networkManager = nullptr;
}

AuthWidget *MainWindow::getAuthWidget() const
{
    return authWidget;
}

OpePanelWidget *MainWindow::getOpePanelWidget() const
{
    return opePanelWidget;
}

SettingWidget *MainWindow::getSettingWidget() const
{
    return settingWidget;
}

NetworkManager *MainWindow::getNetworkManager()
{
    return networkManager;
}

void MainWindow::switchWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return;
    }

    if (stackedWidget->indexOf(widget) < 0) {
        stackedWidget->addWidget(widget);
    }

    stackedWidget->setCurrentWidget(widget);
}
