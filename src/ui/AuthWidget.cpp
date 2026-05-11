#include "AuthWidget.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "mainwindow.h"
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
 * @brief 测试连接按钮点击槽函数
 * @author NAPH130
 */
void AuthWidget::onTestConnection()
{
    // TODO: 测试数据库连接逻辑
}

/**
 * @brief 确认连接按钮点击槽函数
 * @author NAPH130
 */
void AuthWidget::onConfirmConnection()
{
    // 作者：YuzhSong，当前仅预留登录成功信号，后续由连接认证逻辑触发。
    // 为保证本阶段可以手动演示正式页面路由，这里临时发出 loginSucceeded 信号。
    emit loginSucceeded();
}

void AuthWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::authWidget());
}
