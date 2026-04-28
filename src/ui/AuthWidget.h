#pragma once

#include <QWidget>

class MainWindow;
class QCheckBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QToolButton;

/**
 * @class AuthWidget
 * @brief 身份验证页面类
 * @details 负责处理用户登录、注册等验证逻辑
 * @author NAPH1301
 */
class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit AuthWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH1301
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    /**
     * @brief 初始化登录界面
     * @author NAPH1301
     */
    void initializeUi();

    /**
     * @brief 初始化信号与槽连接
     * @author NAPH1301
     */
    void initializeConnections();

    /**
     * @brief 处理显示密码按钮点击事件
     * @author NAPH1301
     */
    void showPasswordButtonClicked();

    /**
     * @brief 处理登录按钮点击事件
     * @author NAPH1301
     */
    void loginButtonClicked();

    /**
     * @brief 处理离线预览按钮点击事件
     * @author NAPH1301
     */
    void offlineButtonClicked();

    /**
     * @brief 处理快速设置按钮点击事件
     * @author NAPH1301
     */
    void settingButtonClicked();

    /**
     * @brief 处理注册按钮点击事件
     * @author NAPH1301
     */
    void registerButtonClicked();

    /**
     * @brief 处理忘记密码按钮点击事件
     * @author NAPH1301
     */
    void forgetPasswordButtonClicked();

    MainWindow *mainWindow; // 主窗口引用
    QFrame *cardFrame; // 登录卡片容器
    QLabel *statusLabel; // 状态提示标签
    QLineEdit *userNameEdit; // 用户名输入框
    QLineEdit *passwordEdit; // 密码输入框
    QLineEdit *serverEdit; // 服务端地址输入框
    QCheckBox *rememberCheckBox; // 记住我选项
    QToolButton *showPasswordButton; // 显示密码按钮
    QPushButton *loginButton; // 登录按钮
    QPushButton *offlineButton; // 离线预览按钮
    QPushButton *settingButton; // 快速设置按钮
    QPushButton *registerButton; // 注册按钮
    QPushButton *forgetPasswordButton; // 忘记密码按钮
};
