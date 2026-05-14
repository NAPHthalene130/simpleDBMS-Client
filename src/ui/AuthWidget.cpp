#include "AuthWidget.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "models/network/NetworkTransferData.h"
#include "network/NetworkManager.h"
#include "network/NetSender.h"
#include "ThemeManager.h"
/**
 * @brief 构造函数
 * @author NAPH130
 * @param mainWindow 主窗口指针
 * @param parent 父部件指针
 */
AuthWidget::AuthWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , ipLineEdit(nullptr)
    , portLineEdit(nullptr)
    , userNameLineEdit(nullptr)
    , passwordLineEdit(nullptr)
    , testConnectionButton(nullptr)
    , confirmButton(nullptr)
    , statusLabel(nullptr)
{
    initUI();
    applyStyles();
}

/**
 * @brief 获取主窗口指针
 * @author NAPH130
 * @return 主窗口指针
 */
MainWindow *AuthWidget::getMainWindow() const
{
    return mainWindow;
}

QString AuthWidget::getIpAddress() const
{
    return ipLineEdit != nullptr ? ipLineEdit->text().trimmed() : QString();
}

QString AuthWidget::getPort() const
{
    return portLineEdit != nullptr ? portLineEdit->text().trimmed() : QString();
}

QString AuthWidget::getUserName() const
{
    return userNameLineEdit != nullptr ? userNameLineEdit->text().trimmed() : QString();
}

QString AuthWidget::getPassword() const
{
    return passwordLineEdit != nullptr ? passwordLineEdit->text() : QString();
}

void AuthWidget::setIpAddress(const QString &ipAddress)
{
    if (ipLineEdit != nullptr) {
        ipLineEdit->setText(ipAddress);
    }
}

void AuthWidget::setPort(const QString &port)
{
    if (portLineEdit != nullptr) {
        portLineEdit->setText(port);
    }
}

void AuthWidget::setUserName(const QString &userName)
{
    if (userNameLineEdit != nullptr) {
        userNameLineEdit->setText(userName);
    }
}

void AuthWidget::setPassword(const QString &password)
{
    if (passwordLineEdit != nullptr) {
        passwordLineEdit->setText(password);
    }
}

/**
 * @brief 初始化界面布局
 * @author NAPH130
 * @details 采用居中卡片式布局，上半部分为表单区，下半部分为按钮区。
 *          IP 与端口同行放置，仿 DataGrip 连接对话框风格。
 */
void AuthWidget::initUI()
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    outerLayout->addStretch();

    auto *cardWidget = new QWidget(this);
    cardWidget->setObjectName("authCard");
    cardWidget->setFixedWidth(420);

    auto *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setContentsMargins(36, 32, 36, 32);
    cardLayout->setSpacing(18);

    auto *headerLabel = new QLabel(tr("连接数据库"), cardWidget);
    headerLabel->setObjectName("authHeaderLabel");
    headerLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(headerLabel);
    cardLayout->addSpacing(8);

    auto *formLayout = new QFormLayout();
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(14);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    auto *ipPortRow = new QHBoxLayout();
    ipPortRow->setSpacing(8);
    ipLineEdit = new QLineEdit(cardWidget);
    ipLineEdit->setPlaceholderText(tr("127.0.0.1"));
    ipLineEdit->setMinimumWidth(180);
    portLineEdit = new QLineEdit(cardWidget);
    portLineEdit->setPlaceholderText(tr("10086"));
    portLineEdit->setFixedWidth(100);
    portLineEdit->setMaxLength(5);
    ipPortRow->addWidget(ipLineEdit, 1);
    ipPortRow->addWidget(portLineEdit, 0);

    auto *ipRowLabel = new QLabel(tr("地址 : 端口"), cardWidget);
    ipRowLabel->setObjectName("formLabel");
    formLayout->addRow(ipRowLabel, ipPortRow);

    userNameLineEdit = new QLineEdit(cardWidget);
    userNameLineEdit->setPlaceholderText(tr("用户名"));
    auto *userLabel = new QLabel(tr("用户"), cardWidget);
    userLabel->setObjectName("formLabel");
    formLayout->addRow(userLabel, userNameLineEdit);

    passwordLineEdit = new QLineEdit(cardWidget);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setPlaceholderText(tr("密码"));
    auto *pwdLabel = new QLabel(tr("密码"), cardWidget);
    pwdLabel->setObjectName("formLabel");
    formLayout->addRow(pwdLabel, passwordLineEdit);

    cardLayout->addLayout(formLayout);
    cardLayout->addSpacing(6);

    // 连接状态提示标签
    // 作者：NAPH130
    statusLabel = new QLabel(QString(), cardWidget);
    statusLabel->setObjectName("authStatusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    cardLayout->addWidget(statusLabel);

    testConnectionButton = new QPushButton(tr("测试连接"), cardWidget);
    testConnectionButton->setObjectName("btnTestConnection");
    testConnectionButton->setCursor(Qt::PointingHandCursor);
    testConnectionButton->setMinimumHeight(36);
    cardLayout->addWidget(testConnectionButton);

    confirmButton = new QPushButton(tr("确认"), cardWidget);
    confirmButton->setObjectName("btnConfirm");
    confirmButton->setCursor(Qt::PointingHandCursor);
    confirmButton->setMinimumHeight(38);
    cardLayout->addWidget(confirmButton);

    outerLayout->addWidget(cardWidget, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    connect(testConnectionButton, &QPushButton::clicked, this, &AuthWidget::onTestConnection);
    connect(confirmButton, &QPushButton::clicked, this, &AuthWidget::onConfirmConnection);

    connect(passwordLineEdit, &QLineEdit::returnPressed, confirmButton, &QPushButton::click);
}

/**
 * @brief 应用全局暗色主题样式
 * @author NAPH130
 */
void AuthWidget::applyStyles()
{
    setStyleSheet(ThemeManager::authWidget());
}

/**
 * @brief 设置连接状态提示文本
 * @author NAPH130
 * @param status 状态文本
 */
void AuthWidget::setConnectionStatus(const QString &status)
{
    if (statusLabel != nullptr) {
        statusLabel->setText(status);
    }
}

/**
 * @brief 测试连接按钮点击槽函数
 * @author NAPH130
 * @details 连接服务端并发送 VERIFY_REQUEST，根据响应显示验证结果
 */
void AuthWidget::onTestConnection()
{
    if (mainWindow == nullptr) {
        return;
    }

    NetworkManager *nm = mainWindow->getNetworkManager();
    if (nm == nullptr) {
        setConnectionStatus(tr("网络模块未初始化"));
        return;
    }

    const QString ip = getIpAddress().isEmpty() ? QStringLiteral("127.0.0.1") : getIpAddress();
    const QString portStr = getPort().isEmpty() ? QStringLiteral("10086") : getPort();
    const unsigned short port = static_cast<unsigned short>(portStr.toUShort());

    // 连接服务端
    // 作者：NAPH130
    if (!nm->connectToServer(ip.toStdString(), port)) {
        setConnectionStatus(tr("无法连接到服务器 %1:%2").arg(ip, portStr));
        return;
    }

    // 启动接收线程
    // 作者：NAPH130
    nm->start();

    // 发送 VERIFY_REQUEST
    // 作者：NAPH130
    NetworkTransferData requestData(NetworkTransferData::VERIFY_REQUEST,
                                    getUserName().toStdString());
    requestData.setPassword(getPassword().toStdString());

    auto serverSocket = nm->getSocket();
    if (serverSocket == nullptr || !serverSocket->is_open()) {
        setConnectionStatus(tr("连接已断开"));
        return;
    }

    NetSender *sender = nm->getNetSender();
    if (sender == nullptr) {
        setConnectionStatus(tr("网络发送器未初始化"));
        return;
    }

    sender->send(serverSocket, requestData.toJson());
    setConnectionStatus(tr("正在验证连接..."));
}

/**
 * @brief 确认连接按钮点击槽函数
 * @author NAPH130
 * @details 确保连接后发送 LOGIN_REQUEST，成功则进入工作区
 */
void AuthWidget::onConfirmConnection()
{
    if (mainWindow == nullptr) {
        return;
    }

    NetworkManager *nm = mainWindow->getNetworkManager();
    if (nm == nullptr) {
        setConnectionStatus(tr("网络模块未初始化"));
        return;
    }

    const QString ip = getIpAddress().isEmpty() ? QStringLiteral("127.0.0.1") : getIpAddress();
    const QString portStr = getPort().isEmpty() ? QStringLiteral("10086") : getPort();
    const unsigned short port = static_cast<unsigned short>(portStr.toUShort());

    // 确保已连接
    // 作者：NAPH130
    bool connected = nm->connectToServer(ip.toStdString(), port);
    if (!connected) {
        setConnectionStatus(tr("无法连接到服务器 %1:%2").arg(ip, portStr));
        return;
    }

    // 确保接收线程已启动
    // 作者：NAPH130
    nm->start();

    // 发送 LOGIN_REQUEST
    // 作者：NAPH130
    NetworkTransferData requestData(NetworkTransferData::LOGIN_REQUEST,
                                    getUserName().toStdString());
    requestData.setPassword(getPassword().toStdString());

    auto serverSocket = nm->getSocket();
    if (serverSocket == nullptr || !serverSocket->is_open()) {
        setConnectionStatus(tr("连接已断开"));
        return;
    }

    NetSender *sender = nm->getNetSender();
    if (sender == nullptr) {
        setConnectionStatus(tr("网络发送器未初始化"));
        return;
    }

    sender->send(serverSocket, requestData.toJson());
    setConnectionStatus(tr("正在登录..."));
}

void AuthWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::authWidget());
}
