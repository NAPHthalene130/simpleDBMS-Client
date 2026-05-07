/**
 * @file SidePanelContainer.h
 * @brief 左侧宽面板容器组件头文件
 * @details 管理 DirectoryWidget 与 LogWidget 的互斥切换和容器收起/展开状态。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class MainWindow;
class QStackedWidget;
class DirectoryWidget;
class LogWidget;

/**
 * @class SidePanelContainer
 * @brief 左侧宽面板容器
 * @details
 * 1. 负责组织 DirectoryWidget（File 面板）与 LogWidget（Log 面板）。
 * 2. 负责 File/Log 互斥显示与“重复点击收起”的状态管理。
 * 3. 不负责业务数据处理，不直接处理 EditorWidget 的打开/保存逻辑。
 * @author YuzhSong
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
        None,   ///< 无激活面板（容器收起）
        File,   ///< File 面板（DirectoryWidget）
        Log     ///< Log 面板（LogWidget）
    };

    /**
     * @brief 构造函数
     * @param mainWindow 主窗口指针，透传给 DirectoryWidget
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit SidePanelContainer(MainWindow* mainWindow, QWidget* parent = nullptr);

    /**
     * @brief 显示 File 面板
     * @author YuzhSong
     */
    void showFilePanel();

    /**
     * @brief 显示 Log 面板
     * @author YuzhSong
     */
    void showLogPanel();

    /**
     * @brief 切换 File 面板（支持重复点击收起）
     * @author YuzhSong
     */
    void toggleFilePanel();

    /**
     * @brief 切换 Log 面板（支持重复点击收起）
     * @author YuzhSong
     */
    void toggleLogPanel();

    /**
     * @brief 收起面板容器
     * @author YuzhSong
     */
    void collapsePanel();

    /**
     * @brief 判断面板容器当前是否可见
     * @return 可见返回 true，否则返回 false
     * @author YuzhSong
     */
    bool isPanelVisible() const;

    /**
     * @brief 获取目录面板组件
     * @return DirectoryWidget 指针
     * @author YuzhSong
     */
    DirectoryWidget* directoryWidget() const;

    /**
     * @brief 获取日志面板组件
     * @return LogWidget 指针
     * @author YuzhSong
     */
    LogWidget* logWidget() const;

    /**
     * @brief 获取当前激活面板类型
     * @return 当前面板类型
     * @author YuzhSong
     */
    PanelType currentPanelType() const;

private:
    /**
     * @brief 初始化界面结构
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化样式
     * @author YuzhSong
     */
    void initStyle();

private:
    MainWindow* mainWindow;               ///< 主窗口引用（仅用于 DirectoryWidget 构造）
    QStackedWidget* stackedWidget;        ///< File/Log 面板切换容器
    DirectoryWidget* m_directoryWidget;   ///< DirectoryWidget 实例
    LogWidget* m_logWidget;               ///< LogWidget 实例
    PanelType m_currentPanelType;         ///< 当前激活面板类型
    bool m_panelVisible;                  ///< 左侧宽面板是否可见
};

