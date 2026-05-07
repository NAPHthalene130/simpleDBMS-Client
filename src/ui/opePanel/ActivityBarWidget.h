/**
 * @file ActivityBarWidget.h
 * @brief IDE 风格左侧 ActivityBar 组件头文件
 * @details 提供窄按钮栏，仅负责发出面板切换请求信号，不负责具体面板的创建与显示控制。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QPushButton;

/**
 * @class ActivityBarWidget
 * @brief 左侧 ActivityBar 按钮栏组件
 * @details
 * 1. 负责显示 File / Log / AI / Terminal 四个按钮并发出对应请求信号。
 * 2. 不负责具体面板内容和布局管理，不直接依赖 DirectoryWidget / LogWidget。
 * 3. 由 OpePanelWidget 作为协调层接收信号并驱动 SidePanelContainer。
 * @author YuzhSong
 */
class ActivityBarWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit ActivityBarWidget(QWidget* parent = nullptr);

    /**
     * @brief 同步按钮选中态
     * @details 由外部协调层在面板状态变化后调用，避免 ActivityBar 与实际面板状态不一致。
     * @param fileActive File 按钮是否高亮
     * @param logActive Log 按钮是否高亮
     * @author YuzhSong
     */
    void updateSelectionState(bool fileActive, bool logActive);

signals:
    /**
     * @brief 请求切换 File 面板信号
     * @author YuzhSong
     */
    void filePanelRequested();

    /**
     * @brief 请求切换 Log 面板信号
     * @author YuzhSong
     */
    void logPanelRequested();

    /**
     * @brief 请求切换 AI 面板信号（预留）
     * @author YuzhSong
     */
    void aiPanelRequested();

    /**
     * @brief 请求切换 Terminal 面板信号（预留）
     * @author YuzhSong
     */
    void terminalPanelRequested();

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

    /**
     * @brief 初始化信号槽连接
     * @author YuzhSong
     */
    void initConnections();

private:
    QPushButton* fileButton;      ///< File 按钮
    QPushButton* logButton;       ///< Log 按钮
    QPushButton* aiButton;        ///< AI 按钮（预留）
    QPushButton* terminalButton;  ///< Terminal 按钮（预留）
};

