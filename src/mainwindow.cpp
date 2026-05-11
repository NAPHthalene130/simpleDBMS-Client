#include "mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFont>
#include <QSettings>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "network/NetworkManager.h"
#include "ui/AuthWidget.h"
#include "ui/OpePanelWidget.h"
#include "ui/SettingWidget.h"
#include "ui/ThemeManager.h"
#include "ui/TopNavigationWidget.h"
#include "ui/opePanel/EditorWidget.h"

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
    const QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
    const bool rememberWindowSize = settings.value("habit/rememberWindowSize", true).toBool();
    if (rememberWindowSize) {
        resize(settings.value("window/size", QSize(1200, 800)).toSize());
    } else {
        resize(1200, 800);
    }

    initUI();
    initStyle();
    initConnections();
    applyGlobalSettings();
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
    const bool connected = networkManager != nullptr
                           && networkManager->getSocket() != nullptr
                           && networkManager->getSocket()->is_open();
    if (!connected && networkManager != nullptr) {
        networkManager->start();
    }

    rootStackedWidget->setCurrentWidget(appContainerWidget);
    appContentStackedWidget->setCurrentWidget(opePanelWidget);
    if (opePanelWidget != nullptr && opePanelWidget->getEditorWidget() != nullptr) {
        opePanelWidget->getEditorWidget()->reloadEditorSettingsFromLocal();
    }
    topNavigationWidget->setUserName(authWidget->getUserName());
    topNavigationWidget->setCurrentPage(TopNavigationWidget::PageType::Workspace);
    setWindowTitle(tr("simpleDBMS - 工作区"));
}

void MainWindow::showSettingPage()
{
    const bool connected = networkManager != nullptr
                           && networkManager->getSocket() != nullptr
                           && networkManager->getSocket()->is_open();
    const QString connectionStatus = connected ? tr("已连接") : tr("未连接");
    const QString currentDatabase = property("currentDatabase").toString();
    settingWidget->refreshAccountInfo(authWidget->getUserName(), connectionStatus, currentDatabase);
    settingWidget->reloadSettingsFromLocal();

    rootStackedWidget->setCurrentWidget(appContainerWidget);
    appContentStackedWidget->setCurrentWidget(settingWidget);
    topNavigationWidget->setUserName(authWidget->getUserName());
    topNavigationWidget->setCurrentPage(TopNavigationWidget::PageType::Setting);
    setWindowTitle(tr("simpleDBMS - 设置"));
}

void MainWindow::logout()
{
    if (networkManager != nullptr) {
        networkManager->stop();
    }
    setProperty("currentDatabase", QString());
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

    connect(settingWidget, &SettingWidget::logoutRequested, this, &MainWindow::logout);
    connect(settingWidget, &SettingWidget::backToWorkspaceRequested, this, &MainWindow::showWorkspacePage);
    connect(settingWidget, &SettingWidget::settingsApplied, this, &MainWindow::applyGlobalSettings);
}

void MainWindow::initStyle()
{
    setStyleSheet(ThemeManager::globalBase());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
    const bool rememberWindowSize = settings.value("habit/rememberWindowSize", true).toBool();
    if (rememberWindowSize) {
        QSettings writableSettings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
        writableSettings.setValue("window/size", size());
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::applyGlobalSettings()
{
    qApp->setStyleSheet(ThemeManager::globalBase());

    const QSettings settings(QStringLiteral("simpleDBMS"), QStringLiteral("simpleDBMS-Client"));
    const int fontSize = settings.value("appearance/uiFontSize", QStringLiteral("14")).toString().toInt();
    if (fontSize > 0) {
        QFont globalFont = qApp->font();
        globalFont.setPixelSize(fontSize);
        qApp->setFont(globalFont);
    }

    const QString scaleStr = settings.value("appearance/uiScale", QStringLiteral("100%")).toString();
    const QString scalePercentStr = scaleStr.chopped(1);
    const int scalePercent = scalePercentStr.toInt();
    if (scalePercent > 0 && scalePercent != 100) {
        const qreal factor = scalePercent / 100.0;
        qputenv("QT_SCALE_FACTOR", QByteArray::number(factor, 'g', 2));
        setStatusTip(tr("界面缩放已设置为 %1，重启应用后完全生效。").arg(QString::number(scalePercent) + "%"));
    } else {
        qunsetenv("QT_SCALE_FACTOR");
    }

    if (authWidget) authWidget->refreshTheme();
    if (opePanelWidget) opePanelWidget->refreshTheme();
    if (settingWidget) settingWidget->refreshTheme();
    if (topNavigationWidget) topNavigationWidget->refreshTheme();
}
