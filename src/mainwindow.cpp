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
    // 初始化主窗口大小，满足项目要求的默认显示尺寸。
    resize(1200, 800);
    setCentralWidget(stackedWidget);

    // 启动后优先展示身份验证页面，作为应用入口页面。
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
