/**
 * @file SettingWidget.h
 * @author YuzhSong
 * @brief 设置页面组件头文件
 * @details 负责显示设置页基础内容与退出登录入口，不负责主窗口页面切换决策
 * @module ui
 */

#pragma once

#include <QWidget>

class MainWindow;
class QLabel;
class QPushButton;

/**
 * @class SettingWidget
 * @author YuzhSong
 * @brief 设置页面组件
 * @details
 * 1. 负责展示设置页标题、说明与退出登录按钮；
 * 2. 不直接操作 MainWindow 的页面切换；
 * 3. 通过 logoutRequested 信号将退出意图交由 MainWindow 统一处理。
 */
class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param mainWindow 主窗口指针，仅用于与现有结构保持兼容，不在本类中直接调用路由
     * @param parent 父组件指针
     */
    explicit SettingWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author YuzhSong
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

signals:
    /**
     * @brief 退出登录请求信号
     * @author YuzhSong
     * @details 点击“退出登录”按钮后发出，由 MainWindow 接收并执行 logout()
     */
    void logoutRequested();

private slots:
    /**
     * @brief 退出登录按钮点击槽函数
     * @author YuzhSong
     * @details 仅发信号，不直接操作页面
     */
    void onLogoutButtonClicked();

private:
    /**
     * @brief 初始化界面布局
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     * @author YuzhSong
     */
    void initConnections();

    /**
     * @brief 初始化样式
     * @author YuzhSong
     */
    void initStyle();

private:
    MainWindow *mainWindow;
    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QPushButton *logoutButton;
};
