#include "mainwindow.h"

#include <QStackedWidget>
#include <QVBoxLayout>

#include "models/network/NetworkTransferData.h"
#include "network/NetSender.h"
#include "network/NetworkManager.h"
#include "ui/AuthWidget.h"
#include "ui/OpePanelWidget.h"
#include "ui/SettingWidget.h"
#include "ui/TopNavigationWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , rootStackedWidget(new QStackedWidget(this))
    , appContainerWidget(new QWidget(this))
    , appContentStackedWidget(new QStackedWidget(this))
    , topNavigationWidget(new TopNavigationWidget(this))
    , authWidget(new AuthWidget(this))
    , opePanelWidget(new OpePanelWidget(this))
    , settingWidget(new SettingWidget(this))
    , networkManager(new NetworkManager(this))
{
    resize(1200, 800);
    initUI();
    initStyle();
    initConnections();
    showAuthPage();
}

MainWindow::~MainWindow()
{
    // 作者：YuzhSong
    // NetworkManager 由 Qt 父子关系自动析构，这里只做语义化置空。
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

    // 作者：YuzhSong
    // 保留原有 switchWidget 对外接口，但内部统一转到语义化路由函数。
    if (widget == authWidget) {
        showAuthPage();
    } else if (widget == opePanelWidget) {
        showWorkspacePage();
    } else if (widget == settingWidget) {
        showSettingPage();
    }
}

void MainWindow::showAuthPage()
{
    rootStackedWidget->setCurrentWidget(authWidget);
    topNavigationWidget->setCurrentPage(TopNavigationWidget::PageType::None);
    setWindowTitle(tr("simpleDBMS - 登录"));
}

void MainWindow::showWorkspacePage()
{
    rootStackedWidget->setCurrentWidget(appContainerWidget);
    appContentStackedWidget->setCurrentWidget(opePanelWidget);
    topNavigationWidget->setCurrentPage(TopNavigationWidget::PageType::Workspace);
    setWindowTitle(tr("simpleDBMS - 工作区"));

    // 进入工作区后自动向服务端请求数据库目录结构
    // 作者：NAPH130
    sendDirectoryRequest();
}

void MainWindow::showSettingPage()
{
    rootStackedWidget->setCurrentWidget(appContainerWidget);
    appContentStackedWidget->setCurrentWidget(settingWidget);
    topNavigationWidget->setCurrentPage(TopNavigationWidget::PageType::Setting);
    setWindowTitle(tr("simpleDBMS - 设置"));
}

void MainWindow::logout()
{
    // 作者：YuzhSong，当前仅处理页面切换，后续可在此接入真实登出逻辑。
    showAuthPage();
}

void MainWindow::initUI()
{
    // 作者：YuzhSong
    // 顶层路由栈：登录页与主应用容器分离，确保 AuthWidget 页面不显示顶部导航栏。
    rootStackedWidget->addWidget(authWidget);

    // 作者：YuzhSong
    // 主应用内容栈：仅管理工作区与设置页，具体切换由 MainWindow 统一调度。
    appContentStackedWidget->addWidget(opePanelWidget);
    appContentStackedWidget->addWidget(settingWidget);

    auto *appContainerLayout = new QVBoxLayout(appContainerWidget);
    appContainerLayout->setContentsMargins(0, 0, 0, 0);
    appContainerLayout->setSpacing(0);
    appContainerLayout->addWidget(topNavigationWidget, 0);
    appContainerLayout->addWidget(appContentStackedWidget, 1);

    rootStackedWidget->addWidget(appContainerWidget);
    setCentralWidget(rootStackedWidget);
}

void MainWindow::initConnections()
{
    // 作者：YuzhSong，当前仅预留登录成功入口，后续由连接认证逻辑触发。
    connect(authWidget, &AuthWidget::loginSucceeded, this, &MainWindow::showWorkspacePage);

    // 作者：YuzhSong
    // TopNavigationWidget 只发请求信号，不直接操作页面；切换中枢保持在 MainWindow。
    connect(topNavigationWidget, &TopNavigationWidget::workspaceRequested, this, &MainWindow::showWorkspacePage);
    connect(topNavigationWidget, &TopNavigationWidget::settingRequested, this, &MainWindow::showSettingPage);

    // 作者：YuzhSong
    // 设置页通过 logoutRequested 发出退出意图，MainWindow 统一执行 logout 路由。
    connect(settingWidget, &SettingWidget::logoutRequested, this, &MainWindow::logout);
}

void MainWindow::initStyle()
{
    setStyleSheet(QString(
        "QMainWindow {"
        "    background-color: #2B2B2B;"
        "}"
    ));
}

/**
 * @brief 向服务端发送目录结构请求
 * @details 构造 DIRECTORY_REQUEST 消息并发送，无参数（取全量目录）。
 * @author NAPH130
 */
void MainWindow::sendDirectoryRequest()
{
    NetworkManager *nm = getNetworkManager();
    if (nm == nullptr || nm->getNetSender() == nullptr) {
        return;
    }
    auto serverSocket = nm->getSocket();
    if (serverSocket == nullptr || !serverSocket->is_open()) {
        return;
    }
    NetworkTransferData requestData(NetworkTransferData::DIRECTORY_REQUEST, std::string());
    nm->getNetSender()->send(serverSocket, requestData.toJson());
}
