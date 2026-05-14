#pragma once

#include <QMainWindow>

class AuthWidget;
class NetworkManager;
class OpePanelWidget;
class QStackedWidget;
class SettingWidget;
class TopNavigationWidget;
class QWidget;
class QCloseEvent;

/**
 * @class MainWindow
 * @brief 应用程序主窗口类
 * @details 负责管理各个页面的切换以及全局窗口状态（如大小、标题等）
 * @author NAPH130
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param parent 父部件指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     * @author NAPH130
     */
    ~MainWindow();

    /**
     * @brief 切换当前显示的页面
     * @author NAPH130
     * @param widget 待显示的页面指针
     */
    void switchWidget(QWidget *widget);

    /**
     * @brief 获取身份验证页面
     * @author NAPH130
     * @return 身份验证页面指针
     */
    AuthWidget *getAuthWidget() const;

    /**
     * @brief 获取操作面板页面
     * @author NAPH130
     * @return 操作面板页面指针
     */
    OpePanelWidget *getOpePanelWidget() const;

    /**
     * @brief 获取设置页面
     * @author NAPH130
     * @return 设置页面指针
     */
    SettingWidget *getSettingWidget() const;

    /**
     * @brief 获取网络管理器
     * @author NAPH130
     * @return 网络管理器指针
     */
    NetworkManager *getNetworkManager();

    /**
     * @brief 获取顶部导航栏
     * @author NAPH130
     * @return 顶部导航栏指针
     */
    TopNavigationWidget *getTopNavigationWidget() const;

    /**
     * @brief 显示登录认证页
     * @author YuzhSong
     * @details
     * 1. 将主窗口切换到 AuthWidget 页面；
     * 2. 该页面不显示顶部导航栏；
     * 3. 会同步更新窗口标题语义。
     */
    void showAuthPage();

    /**
     * @brief 显示工作区页面
     * @author YuzhSong
     * @details
     * 1. 切换到主应用容器；
     * 2. 在主应用内容栈中显示 OpePanelWidget；
     * 3. 同步顶部导航栏按钮高亮状态。
     */
    void showWorkspacePage();

    /**
     * @brief 显示设置页面
     * @author YuzhSong
     * @details
     * 1. 切换到主应用容器；
     * 2. 在主应用内容栈中显示 SettingWidget；
     * 3. 同步顶部导航栏按钮高亮状态。
     */
    void showSettingPage();

    /**
     * @brief 显示表内容视图
     * @author NAPH130
     * @details 切换主工作区到 TableWidget 视图，对应顶部导航"表内容"标签
     */
    void showTablePage();

    /**
     * @brief 退出登录并返回认证页
     * @author YuzhSong
     * @details
     * 当前阶段仅执行前端页面路由回退，不触发真实网络登出逻辑。
     */
    void logout();

    /**
     * @brief 连接丢失时回调，跳转至登录页并提示
     * @author NAPH130
     */
    void onConnectionLost();

    /**
     * @brief 向服务端发送目录结构请求
     * @author NAPH130
     * @details 构造 DIRECTORY_REQUEST 协议消息并通过 NetSender 发送，服务端响应后
     *          由 NetReceiver 分发至 DirectoryWidget 刷新界面。
     */
    void sendDirectoryRequest();

    /**
     * @brief 应用全局设置（主题/字号/缩放）
     * @author Qi
     * @details 从 QSettings 读取并即时应用主题、字号、缩放
     */
    void applyGlobalSettings();

private:
    /**
     * @brief 初始化主窗口 UI 结构
     * @author YuzhSong
     * @details
     * 结构为：
     * 1. rootStackedWidget：Auth 页 / 主应用容器；
     * 2. 主应用容器：TopNavigationWidget + appContentStackedWidget。
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     * @author YuzhSong
     * @details
     * 统一收敛页面切换入口，避免依赖 Debug 页面切换。
     */
    void initConnections();

    /**
     * @brief 初始化主窗口样式
     * @author YuzhSong
     */
    void initStyle();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QStackedWidget *rootStackedWidget;
    QWidget *appContainerWidget;
    QStackedWidget *appContentStackedWidget;
    TopNavigationWidget *topNavigationWidget;
    AuthWidget *authWidget;
    OpePanelWidget *opePanelWidget;
    SettingWidget *settingWidget;
    NetworkManager *networkManager;
};
