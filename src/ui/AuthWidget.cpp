#include "authwidget.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTimer>

// 模拟数据模式，不依赖 Qt SQL
#define SIMULATE_DB 1

AuthWidget::AuthWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();

    // 设置默认值
    m_hostEdit->setText("localhost");
    m_portEdit->setText("3306");
    m_userEdit->setText("root");
}

AuthWidget::~AuthWidget()
{
}

void AuthWidget::setupUI()
{
    setWindowTitle("连接数据库 (模拟模式)");
    setFixedSize(320, 220);

    QFormLayout *formLayout = new QFormLayout;
    m_hostEdit = new QLineEdit(this);
    m_portEdit = new QLineEdit(this);
    m_userEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("主机:", m_hostEdit);
    formLayout->addRow("端口:", m_portEdit);
    formLayout->addRow("用户名:", m_userEdit);
    formLayout->addRow("密码:", m_passwordEdit);

    m_loginBtn = new QPushButton("登录", this);
    m_testBtn = new QPushButton("测试连接", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_testBtn);
    btnLayout->addWidget(m_loginBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    connect(m_loginBtn, &QPushButton::clicked, this, &AuthWidget::onLoginClicked);
    connect(m_testBtn, &QPushButton::clicked, this, &AuthWidget::onTestConnection);
}

void AuthWidget::onTestConnection()
{
#if SIMULATE_DB
    QMessageBox::information(this, "模拟模式", "连接测试成功！(模拟数据)");
#else
    // 真实数据库代码（需要 Qt SQL 模块，此处留空）
#endif
}

void AuthWidget::onLoginClicked()
{
#if SIMULATE_DB
    // 模拟延迟后发送登录成功信号
    QMessageBox::information(this, "模拟模式", "登录成功！使用模拟数据。");
    QTimer::singleShot(100, this, [this]() {
        emit loginSuccess(m_hostEdit->text(),
                          m_portEdit->text().toInt(),
                          m_userEdit->text(),
                          m_passwordEdit->text());
    });
#else
    // 真实数据库代码（需要 Qt SQL 模块，此处留空）
#endif
}