/**
 * @file SidePanelContainer.h
 * @author YuzhSong
 * @brief 左侧宽面板容器头文件
 * @details 使用 QStackedWidget 管理 Directory/File/Log 三个互斥面板
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class MainWindow;
class QStackedWidget;
class DirectoryWidget;
class FileWidget;
class LogWidget;

/**
 * @class SidePanelContainer
 * @author YuzhSong
 * @brief 左侧宽面板容器
 * @details
 * 1. 负责管理 DirectoryWidget、FileWidget、LogWidget 的互斥显示。
 * 2. 重复点击当前激活入口时收起面板，符合 IDE 侧栏交互习惯。
 * 3. 不承载 SQL 编辑器、自动补全、服务端通信等核心业务逻辑。
 */
class SidePanelContainer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 面板类型枚举
     * @author YuzhSong
     */
    enum class PanelType {
        None,       ///< 当前无激活面板（容器收起）
        Directory,  ///< 数据库目录树面板
        File,       ///< 已打开文件列表面板
        Log         ///< 本地操作日志面板
    };

    /**
     * @brief 构造函数
     * @param mainWindow 主窗口指针（当前仅保留构造签名兼容）
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit SidePanelContainer(MainWindow* mainWindow, QWidget* parent = nullptr);

    void showDirectoryPanel();
    void showFilePanel();
    void showLogPanel();

    void toggleDirectoryPanel();
    void toggleFilePanel();
    void toggleLogPanel();

    void collapsePanel();

    bool isPanelVisible() const;

    DirectoryWidget* directoryWidget() const;
    FileWidget* fileWidget() const;
    LogWidget* logWidget() const;

    PanelType currentPanelType() const;

    void refreshTheme();

private:
    void initUI();
    void initStyle();

private:
    MainWindow* mainWindow;               ///< 主窗口引用（保留兼容，不参与核心逻辑）
    QStackedWidget* stackedWidget;        ///< 三面板切换容器
    DirectoryWidget* m_directoryWidget;   ///< 数据库目录树面板
    FileWidget* m_fileWidget;             ///< 已打开文件列表面板
    LogWidget* m_logWidget;               ///< 本地日志面板
    PanelType m_currentPanelType;         ///< 当前激活面板类型
    bool m_panelVisible;                  ///< 面板容器可见性
};

