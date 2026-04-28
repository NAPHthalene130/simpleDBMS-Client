#include "AuthWidget.h"

#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

AuthWidget::AuthWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      cardFrame(nullptr),
      statusLabel(nullptr),
      userNameEdit(nullptr),
      passwordEdit(nullptr),
      serverEdit(nullptr),
      rememberCheckBox(nullptr),
      showPasswordButton(nullptr),
      loginButton(nullptr),
      offlineButton(nullptr),
      settingButton(nullptr),
      registerButton(nullptr),
      forgetPasswordButton(nullptr)
{
    initializeUi();
    initializeConnections();
}

void AuthWidget::initializeUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *heroLayout = new QHBoxLayout();
    auto *heroBadgeLabel = new QLabel(tr("登录门户"), this);
    auto *topHintLabel = new QLabel(tr("simpleDBMS 客户端"), this);
    cardFrame = new QFrame(this);
    auto *cardLayout = new QVBoxLayout(cardFrame);
    auto *titleLabel = new QLabel(tr("欢迎回来"), cardFrame);
    auto *subtitleLabel = new QLabel(tr("连接数据库、进入工作区、继续你的开发流程。"), cardFrame);
    auto *fieldHintLabel = new QLabel(tr("请先填写连接信息。按钮事件已预留，后续可在方法中补业务逻辑。"), cardFrame);
    userNameEdit = new QLineEdit(cardFrame);
    passwordEdit = new QLineEdit(cardFrame);
    serverEdit = new QLineEdit(cardFrame);
    auto *databaseNameEdit = new QLineEdit(cardFrame);
    auto *optionsLayout = new QHBoxLayout();
    rememberCheckBox = new QCheckBox(tr("记住我"), cardFrame);
    showPasswordButton = new QToolButton(cardFrame);
    loginButton = new QPushButton(tr("登录"), cardFrame);
    registerButton = new QPushButton(tr("注册"), cardFrame);
    forgetPasswordButton = new QPushButton(tr("忘记密码"), cardFrame);
    auto *quickActionLayout = new QHBoxLayout();
    offlineButton = new QPushButton(tr("离线预览"), cardFrame);
    settingButton = new QPushButton(tr("快速设置"), cardFrame);
    auto *assistLayout = new QHBoxLayout();
    auto *securityHintLabel = new QLabel(tr("安全提示：建议连接实验环境时单独配置服务器地址。"), cardFrame);
    statusLabel = new QLabel(tr("当前状态：未连接"), cardFrame);
    auto *footerLabel = new QLabel(tr("提示：你可以尝试不同的服务器地址以适配本地或实验环境。"), cardFrame);

    setObjectName("authWidget");
    heroBadgeLabel->setObjectName("authHeroBadgeLabel");
    cardFrame->setObjectName("authCard");
    titleLabel->setObjectName("authTitleLabel");
    subtitleLabel->setObjectName("authSubtitleLabel");
    fieldHintLabel->setObjectName("authFieldHintLabel");
    topHintLabel->setObjectName("authTopHintLabel");
    securityHintLabel->setObjectName("authSecurityHintLabel");
    statusLabel->setObjectName("authStatusLabel");
    footerLabel->setObjectName("authFooterLabel");
    loginButton->setObjectName("authPrimaryButton");
    offlineButton->setObjectName("authSecondaryButton");
    settingButton->setObjectName("authSecondaryButton");
    registerButton->setObjectName("authSecondaryButton");
    forgetPasswordButton->setObjectName("authTextButton");

    heroBadgeLabel->setAlignment(Qt::AlignCenter);
    topHintLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    fieldHintLabel->setWordWrap(true);
    securityHintLabel->setWordWrap(true);
    footerLabel->setWordWrap(true);
    statusLabel->setAlignment(Qt::AlignCenter);

    userNameEdit->setPlaceholderText(tr("用户名"));
    passwordEdit->setPlaceholderText(tr("密码"));
    serverEdit->setPlaceholderText(tr("服务器地址，例如 127.0.0.1:10086"));
    databaseNameEdit->setPlaceholderText(tr("默认数据库，例如 simple_db"));
    passwordEdit->setEchoMode(QLineEdit::Password);

    showPasswordButton->setText(tr("显示"));
    showPasswordButton->setCursor(Qt::PointingHandCursor);
    loginButton->setCursor(Qt::PointingHandCursor);
    offlineButton->setCursor(Qt::PointingHandCursor);
    settingButton->setCursor(Qt::PointingHandCursor);
    registerButton->setCursor(Qt::PointingHandCursor);
    forgetPasswordButton->setCursor(Qt::PointingHandCursor);

    cardFrame->setFixedWidth(420);

    heroLayout->addStretch();
    heroLayout->addWidget(heroBadgeLabel);
    heroLayout->addStretch();

    optionsLayout->addWidget(rememberCheckBox);
    optionsLayout->addStretch();
    optionsLayout->addWidget(showPasswordButton);

    assistLayout->addWidget(forgetPasswordButton);
    assistLayout->addStretch();
    assistLayout->addWidget(registerButton);

    quickActionLayout->addWidget(offlineButton);
    quickActionLayout->addWidget(settingButton);

    cardLayout->setSpacing(14);
    cardLayout->setContentsMargins(28, 28, 28, 28);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(subtitleLabel);
    cardLayout->addWidget(fieldHintLabel);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(userNameEdit);
    cardLayout->addWidget(passwordEdit);
    cardLayout->addWidget(serverEdit);
    cardLayout->addWidget(databaseNameEdit);
    cardLayout->addLayout(optionsLayout);
    cardLayout->addWidget(statusLabel);
    cardLayout->addWidget(loginButton);
    cardLayout->addLayout(assistLayout);
    cardLayout->addLayout(quickActionLayout);
    cardLayout->addWidget(securityHintLabel);
    cardLayout->addWidget(footerLabel);

    mainLayout->setContentsMargins(48, 32, 48, 32);
    mainLayout->addLayout(heroLayout);
    mainLayout->addWidget(topHintLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(cardFrame, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    setStyleSheet(
        "#authWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f8fbff, stop:1 #eef3ff);"
        "}"
        "#authTopHintLabel {"
        "    color: #5b6475;"
        "    font-size: 14px;"
        "    letter-spacing: 1px;"
        "}"
        "#authHeroBadgeLabel {"
        "    min-width: 120px;"
        "    padding: 6px 14px;"
        "    border-radius: 999px;"
        "    background-color: #eef3ff;"
        "    color: #5260f0;"
        "    font-size: 12px;"
        "    font-weight: 700;"
        "}"
        "#authCard {"
        "    background-color: rgba(255, 255, 255, 235);"
        "    border: 1px solid #dbe5f4;"
        "    border-radius: 24px;"
        "}"
        "#authTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 28px;"
        "    font-weight: 700;"
        "}"
        "#authFieldHintLabel {"
        "    color: #7c8698;"
        "    font-size: 12px;"
        "    padding: 8px 12px;"
        "    background-color: #f8fbff;"
        "    border: 1px solid #e3ebf8;"
        "    border-radius: 10px;"
        "}"
        "#authSubtitleLabel, #authFooterLabel, #authSecurityHintLabel {"
        "    color: #6b7280;"
        "    font-size: 13px;"
        "}"
        "#authStatusLabel {"
        "    color: #3550d8;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "    padding: 8px 12px;"
        "    background-color: #f4f7ff;"
        "    border: 1px solid #dbe5f4;"
        "    border-radius: 10px;"
        "}"
        "QLineEdit {"
        "    min-height: 42px;"
        "    padding: 0 14px;"
        "    border: 1px solid #d5dded;"
        "    border-radius: 12px;"
        "    background-color: #ffffff;"
        "    selection-background-color: #6d8cff;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #7c96ff;"
        "}"
        "QCheckBox {"
        "    color: #4b5563;"
        "}"
        "QToolButton {"
        "    color: #5d6bff;"
        "    background: transparent;"
        "    border: none;"
        "    font-weight: 600;"
        "}"
        "#authPrimaryButton {"
        "    min-height: 44px;"
        "    border: none;"
        "    border-radius: 12px;"
        "    background-color: #5d6bff;"
        "    color: white;"
        "    font-weight: 700;"
        "}"
        "#authPrimaryButton:hover {"
        "    background-color: #5260f0;"
        "}"
        "#authSecondaryButton {"
        "    min-height: 40px;"
        "    border-radius: 12px;"
        "    border: 1px solid #d7def0;"
        "    background-color: #f9fbff;"
        "    color: #334155;"
        "    font-weight: 600;"
        "}"
        "#authSecondaryButton:hover {"
        "    background-color: #f1f5ff;"
        "}"
        "#authTextButton {"
        "    min-height: 32px;"
        "    border: none;"
        "    background: transparent;"
        "    color: #5d6bff;"
        "    font-weight: 600;"
        "}"
    );
}

void AuthWidget::initializeConnections()
{
    connect(showPasswordButton, &QToolButton::clicked, this, [this]() {
        showPasswordButtonClicked();
    });

    connect(settingButton, &QPushButton::clicked, this, [this]() {
        settingButtonClicked();
    });

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        loginButtonClicked();
    });

    connect(offlineButton, &QPushButton::clicked, this, [this]() {
        offlineButtonClicked();
    });

    connect(registerButton, &QPushButton::clicked, this, [this]() {
        registerButtonClicked();
    });

    connect(forgetPasswordButton, &QPushButton::clicked, this, [this]() {
        forgetPasswordButtonClicked();
    });
}

MainWindow *AuthWidget::getMainWindow() const
{
    return mainWindow;
}

void AuthWidget::showPasswordButtonClicked()
{
    // TODO: 在此处补充显示密码按钮点击后的处理逻辑。
}

void AuthWidget::loginButtonClicked()
{
    // TODO: 在此处补充登录按钮点击后的处理逻辑。
}

void AuthWidget::offlineButtonClicked()
{
    // TODO: 在此处补充离线预览按钮点击后的处理逻辑。
}

void AuthWidget::settingButtonClicked()
{
    // TODO: 在此处补充快速设置按钮点击后的处理逻辑。
}

void AuthWidget::registerButtonClicked()
{
    // TODO: 在此处补充注册按钮点击后的处理逻辑。
}

void AuthWidget::forgetPasswordButtonClicked()
{
    // TODO: 在此处补充忘记密码按钮点击后的处理逻辑。
}
