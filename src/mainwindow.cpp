#include "mainwindow.h"
#include "ui/authwidget.h"      // 修改为 ui/ 路径
#include "ui/contentwidget.h"   // 修改为 ui/ 路径

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SimpleDBMS Client (模拟模式)");
    resize(1000, 600);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // 创建登录界面并设为当前页
    AuthWidget *auth = new AuthWidget(this);
    m_stackedWidget->addWidget(auth);

    // 登录成功后切换到内容界面
    connect(auth, &AuthWidget::loginSuccess,
            [this](const QString &host, int port,
                   const QString &user, const QString &password) {
        ContentWidget *content = new ContentWidget(host, port, user, password, this);
        m_stackedWidget->addWidget(content);
        m_stackedWidget->setCurrentWidget(content);
    });
}

MainWindow::~MainWindow()
{
}
