#pragma once

#include <QWidget>

class MainWindow;
class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @class AuthWidget
 * @brief 数据库连接登录页面
 * @details 提供 IP、端口、用户名、密码的输入表单，以及测试连接和确认按钮，
 *          仿 DataGrip 风格的数据库连接界面。
 * @author NAPH130
 */
class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针，用于后续页面跳转
     * @param parent 父部件指针
     */
    explicit AuthWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH130
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

    /**
     * @brief 获取当前 IP 地址输入
     * @author NAPH130
     * @return IP 地址文本
     */
    QString getIpAddress() const;

    /**
     * @brief 获取当前端口号输入
     * @author NAPH130
     * @return 端口号文本
     */
    QString getPort() const;

    /**
     * @brief 获取当前用户名输入
     * @author NAPH130
     * @return 用户名文本
     */
    QString getUserName() const;

    /**
     * @brief 获取当前密码输入
     * @author NAPH130
     * @return 密码文本
     */
    QString getPassword() const;

    /**
     * @brief 设置 IP 地址输入
     * @author NAPH130
     * @param ipAddress IP 地址文本
     */
    void setIpAddress(const QString &ipAddress);

    /**
     * @brief 设置端口号输入
     * @author NAPH130
     * @param port 端口号文本
     */
    void setPort(const QString &port);

    /**
     * @brief 设置用户名输入
     * @author NAPH130
     * @param userName 用户名文本
     */
    void setUserName(const QString &userName);

    /**
     * @brief 设置密码输入
     * @author NAPH130
     * @param password 密码文本
     */
    void setPassword(const QString &password);

    /**
     * @brief 设置连接状态提示文本
     * @author NAPH130
     * @param status 状态文本
     */
    void setConnectionStatus(const QString &status);

signals:
    /**
     * @brief 登录成功信号
     * @author YuzhSong
     * @details
     * 当前阶段仅预留页面路由入口，不包含真实认证过程。
     * MainWindow 接收该信号后负责切换到工作区页面。
     */
    void loginSucceeded();

private slots:
    /**
     * @brief 测试连接按钮点击槽函数
     * @author NAPH130
     */
    void onTestConnection();

    /**
     * @brief 确认连接按钮点击槽函数
     * @author NAPH130
     */
    void onConfirmConnection();

private:
    /**
     * @brief 初始化界面布局
     * @author NAPH130
     */
    void initUI();

    /**
     * @brief 应用全局暗色主题样式
     * @author NAPH130
     */
    void applyStyles();

    MainWindow *mainWindow;

    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *userNameLineEdit;
    QLineEdit *passwordLineEdit;

    QPushButton *testConnectionButton;
    QPushButton *confirmButton;
    QLabel *statusLabel;
};
