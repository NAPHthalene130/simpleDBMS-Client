/**
 * @file OpePanelWidget.h
 * @brief 操作工作区主容器头文件
 * @details 负责组织 ActivityBar、左侧面板容器、中间编辑区和底部终端区的布局与基础联动。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui
 */

#pragma once

#include <QWidget>

class MainWindow;
class QSplitter;
class QStackedWidget;
class DirectoryWidget;
class EditorWidget;
class TableWidget;
class TerminalWidget;
class ActivityBarWidget;
class SidePanelContainer;

/**
 * @class OpePanelWidget
 * @brief 操作工作区主容器
 * @details
 * 1. 负责 IDE 风格布局编排与组件之间信号槽协调。
 * 2. 不负责 EditorWidget / DirectoryWidget / TerminalWidget 的核心业务逻辑实现。
 * 3. 通过 SidePanelContainer 管理 File/Log 面板互斥切换与收起行为。
 * @author YuzhSong
 */
class OpePanelWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param mainWindow 主窗口指针
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit OpePanelWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    /**
     * @brief 切换中央主显示区组件
     * @param widget 目标显示组件
     * @author YuzhSong
     */
    void switchWidget(QWidget* widget);

    /**
     * @brief 获取主窗口指针
     * @return 主窗口指针
     * @author YuzhSong
     */
    MainWindow* getMainWindow() const;

    /**
     * @brief 获取目录组件（由 SidePanelContainer 持有）
     * @return 目录组件指针
     * @author YuzhSong
     */
    DirectoryWidget* getDirectoryWidget() const;

    /**
     * @brief 获取编辑器组件
     * @return 编辑器组件指针
     * @author YuzhSong
     */
    EditorWidget* getEditorWidget() const;

    /**
     * @brief 获取表格组件
     * @return 表格组件指针
     * @author YuzhSong
     */
    TableWidget* getTableWidget() const;

    /**
     * @brief 获取终端组件
     * @return 终端组件指针
     * @author YuzhSong
     */
    TerminalWidget* getTerminalWidget() const;

    /**
     * @brief 获取中央主显示区容器
     * @return 中央主显示区容器指针
     * @author YuzhSong
     */
    QStackedWidget* getMainDisplayStackedWidget() const;

public slots:
    /**
     * @brief 切换 File 面板显示状态（兼容旧入口）
     * @author YuzhSong
     */
    void toggleDirectoryPanel();

    /**
     * @brief 收起左侧宽面板（兼容旧入口）
     * @author YuzhSong
     */
    void collapseDirectoryPanel();

    /**
     * @brief 展开左侧宽面板并显示 File 面板（兼容旧入口）
     * @author YuzhSong
     */
    void expandDirectoryPanel();

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
     * @brief 同步 ActivityBar 按钮选中态
     * @author YuzhSong
     */
    void syncActivityBarState();

private:
    MainWindow* mainWindow;                    ///< 主窗口引用
    ActivityBarWidget* activityBarWidget;      ///< 最左侧 ActivityBar
    SidePanelContainer* sidePanelContainer;    ///< 左侧宽面板容器（File/Log）
    EditorWidget* editorWidget;                ///< 中央编辑器
    TableWidget* tableWidget;                  ///< 中央表格组件
    TerminalWidget* terminalWidget;            ///< 底部终端
    QStackedWidget* mainDisplayStackedWidget;  ///< 中央主显示区容器（Editor/Table）
    QWidget* rightPanelWidget;                 ///< 右侧容器（中央区 + 终端）
    QSplitter* horizontalSplitter;             ///< 水平分割器（ActivityBar / SidePanel / RightPanel）
};

