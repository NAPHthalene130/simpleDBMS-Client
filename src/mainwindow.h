#pragma once

#include <QMainWindow>

class AuthWidget;
class NetworkManager;
class OpePanelWidget;
class QStackedWidget;
class SettingWidget;
class QWidget;

/**
 * @class MainWindow
 * @brief 应用程序主窗口类
 * @details 负责管理各个页面的切换以及全局窗口状态（如大小、标题等）
 * @author NAPH1301
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param parent 父部件指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     * @author NAPH1301
     */
    ~MainWindow();

    /**
     * @brief 切换当前显示的页面
     * @author NAPH1301
     * @param widget 待显示的页面指针
     */
    void switchWidget(QWidget *widget);

    /**
     * @brief 获取身份验证页面
     * @author NAPH1301
     * @return 身份验证页面指针
     */
    AuthWidget *getAuthWidget() const;

    /**
     * @brief 获取操作面板页面
     * @author NAPH1301
     * @return 操作面板页面指针
     */
    OpePanelWidget *getOpePanelWidget() const;

    /**
     * @brief 获取设置页面
     * @author NAPH1301
     * @return 设置页面指针
     */
    SettingWidget *getSettingWidget() const;

    /**
     * @brief 获取网络管理器
     * @author NAPH1301
     * @return 网络管理器指针
     */
    NetworkManager *getNetworkManager();

private:
    QStackedWidget *stackedWidget;
    AuthWidget *authWidget;
    OpePanelWidget *opePanelWidget;
    SettingWidget *settingWidget;
    NetworkManager *networkManager;
};
