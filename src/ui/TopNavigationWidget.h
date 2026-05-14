/**
 * @file TopNavigationWidget.h
 * @author YuzhSong
 * @brief 顶部导航栏组件头文件
 * @details 负责主应用区顶部导航展示与页面切换请求发起，不负责具体页面切换执行
 * @module ui
 */

#pragma once

#include <QWidget>

class QLabel;
class QTabBar;

/**
 * @class TopNavigationWidget
 * @author YuzhSong
 * @brief 主应用区顶部导航栏
 * @details
 * 1. 负责展示品牌标识、工作区按钮、设置按钮、用户名区域；
 * 2. 负责发出导航请求信号，不直接操作 QStackedWidget；
 * 3. 由 MainWindow 在非 AuthWidget 页面中托管显示。
 */
class TopNavigationWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 页面类型枚举
     * @author YuzhSong
     */
    enum class PageType {
        None,
        Workspace,
        TableView,
        Setting
    };

    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param parent 父组件指针
     */
    explicit TopNavigationWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置当前高亮页面
     * @author YuzhSong
     * @param pageType 当前页面类型
     * @details 会更新按钮选中态样式，不触发页面跳转。
     */
    void setCurrentPage(PageType pageType);

    /**
     * @brief 设置右侧用户名文本
     * @author YuzhSong
     * @param userName 用户名文本，空字符串时回退为默认值
     */
    void setUserName(const QString &userName);

    void refreshTheme();

signals:
    /**
     * @brief 请求切换到工作区
     * @author YuzhSong
     */
    void workspaceRequested();

    /**
     * @brief 请求切换到表内容视图
     * @author NAPH130
     */
    void tableViewRequested();

    /**
     * @brief 请求切换到设置页
     * @author YuzhSong
     */
    void settingRequested();

private slots:
    void onTabChanged(int index);

private:
    void initUI();
    void initConnections();
    void initStyle();
    void updateTabStyle();

private:
    PageType currentPageType;
    QLabel *brandLabel;
    QTabBar *navigationTabBar;
    QLabel *userNamePrefixLabel;
    QLabel *userNameValueLabel;
};
