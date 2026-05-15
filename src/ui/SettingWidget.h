/**
 * @file SettingWidget.h
 * @author Qi
 * @brief 设置页面组件头文件
 * @details 负责展示 IDE 风格设置页面并进行本地配置保存，不负责后端业务逻辑
 * @module ui
 */

#pragma once

#include <QSettings>
#include <QWidget>

class MainWindow;
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLayout;
class QListWidget;
class QPushButton;
class QStackedWidget;

/**
 * @class SettingWidget
 * @author Qi
 * @brief 设置页面组件
 * @details
 * 1. 提供“外观 / 编辑器 / 使用习惯 / 账户”四类设置；
 * 2. 使用 QSettings 本地保存配置；
 * 3. 通过信号将“退出登录 / 返回工作区 / 设置已应用”意图交由 MainWindow 统一处理。
 */
class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author Qi
     * @param mainWindow 主窗口指针
     * @param parent 父组件指针
     */
    explicit SettingWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author Qi
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

    /**
     * @brief 刷新账户信息展示
     * @author Qi
     * @param currentUser 当前用户名
     * @param connectionStatus 当前连接状态文本
     * @param currentDatabase 当前数据库名
     */
    void refreshAccountInfo(const QString &currentUser,
                            const QString &connectionStatus,
                            const QString &currentDatabase);

    /**
     * @brief 从本地重新加载设置到界面
     * @author Qi
     */
    void reloadSettingsFromLocal();

    void refreshTheme();

signals:
    /**
     * @brief 退出登录请求信号
     * @author Qi
     */
    void logoutRequested();

    /**
     * @brief 返回工作区请求信号
     * @author Qi
     * @details 点击“取消/确定”后发出，由 MainWindow 执行页面切换
     */
    void backToWorkspaceRequested();

    /**
     * @brief 设置已应用信号
     * @author Qi
     * @details 点击“应用/确定”后发出，通知 MainWindow 重新加载全局设置
     */
    void settingsApplied();

    /**
     * @brief 请求重启客户端信号
     * @author Qi
     * @details 切换主题时用户选择立即重启后发出
     */
    void restartRequested();

private slots:
    /**
     * @brief 处理分类切换
     * @author Qi
     * @param categoryIndex 分类索引
     */
    void onCategoryChanged(int categoryIndex);

    void onApplyButtonClicked();
    void onOkButtonClicked();
    void onCancelButtonClicked();
    void onResetButtonClicked();
    void onLogoutButtonClicked();

private:
    void initUI();
    void initConnections();
    void initStyle();

    QWidget *buildAppearancePage();
    QWidget *buildEditorPage();
    QWidget *buildHabitPage();
    QWidget *buildAccountPage();

    QSettings buildSettingsStorage() const;
    void loadSettingsToUi();
    void saveSettingsFromUi();
    void saveSettingsFromUiExceptTheme();
    void resetUiToDefaults();
    bool isThemeChanged() const;

    void setStatusMessage(const QString &message);
    QFrame *buildGroupCard(const QString &title, QLayout *contentLayout) const;

private:
    MainWindow *mainWindow;
    QLabel *titleLabel;
    QLabel *statusLabel;

    QListWidget *categoryListWidget;
    QStackedWidget *contentStackedWidget;

    QComboBox *themeComboBox;
    QComboBox *uiFontSizeComboBox;
    QComboBox *uiScaleComboBox;

    QComboBox *editorFontFamilyComboBox;
    QComboBox *editorFontSizeComboBox;
    QCheckBox *editorAutoWrapCheckBox;
    QCheckBox *editorShowLineNumberCheckBox;

    QCheckBox *autoSwitchToTableCheckBox;
    QCheckBox *keepEditorContentCheckBox;
    QCheckBox *dangerousSqlWarningCheckBox;
    QCheckBox *rememberWindowSizeCheckBox;
    QCheckBox *rememberDatabaseCheckBox;

    QLabel *currentUserValueLabel;
    QLabel *connectionStatusValueLabel;
    QLabel *currentDatabaseValueLabel;
    QPushButton *logoutButton;

    QPushButton *resetButton;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QPushButton *okButton;
};
