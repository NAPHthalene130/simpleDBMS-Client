/**
 * @file OpePanelWidget.h
 * @author YuzhSong
 * @brief 操作工作区主容器头文件
 * @details 负责协调 ActivityBar、SidePanel、Editor、AI 面板和 Terminal 的布局与显隐，不承载业务逻辑
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
class AiPanelWidget;

/**
 * @class OpePanelWidget
 * @author YuzhSong
 * @brief IDE 风格工作区主容器
 * @details
 * 1. 负责布局结构与面板显隐协调。
 * 2. 不负责 EditorWidget / DirectoryWidget / TerminalWidget 核心业务逻辑。
 * 3. 通过信号槽保持组件间解耦。
 */
class OpePanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpePanelWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    void switchWidget(QWidget* widget);

    MainWindow* getMainWindow() const;
    DirectoryWidget* getDirectoryWidget() const;
    EditorWidget* getEditorWidget() const;
    TableWidget* getTableWidget() const;
    TerminalWidget* getTerminalWidget() const;
    QStackedWidget* getMainDisplayStackedWidget() const;

public slots:
    void toggleDirectoryPanel();
    void collapseDirectoryPanel();
    void expandDirectoryPanel();

private:
    // 作者：YuzhSong
    // 统一关键默认尺寸，确保折叠后再次展开恢复程序默认值，而不是恢复用户拖动后的历史尺寸。
    static constexpr int ACTIVITY_BAR_WIDTH = 48;
    static constexpr int SIDE_PANEL_DEFAULT_WIDTH = 220;
    static constexpr int AI_PANEL_DEFAULT_WIDTH = 280;
    static constexpr int TERMINAL_DEFAULT_HEIGHT = 180;

    /**
     * @brief 初始化界面布局
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     */
    void initConnections();

    /**
     * @brief 初始化容器样式
     */
    void initStyle();

    /**
     * @brief 切换 File 面板显示状态
     */
    void toggleFilePanel();

    /**
     * @brief 切换 Log 面板显示状态
     */
    void toggleLogPanel();

    /**
     * @brief 切换 AI 面板显示状态
     */
    void toggleAiPanel();

    /**
     * @brief 展开 SidePanel，并恢复默认宽度
     */
    void showSidePanelWithDefaultWidth();

    /**
     * @brief 折叠 SidePanel
     */
    void collapseSidePanel();

    /**
     * @brief 展开 AI 面板，并恢复默认宽度
     */
    void showAiPanelWithDefaultWidth();

    /**
     * @brief 折叠 AI 面板
     */
    void collapseAiPanel();

    /**
     * @brief 切换 Terminal 面板显示状态
     */
    void toggleTerminalPanel();

    /**
     * @brief 展开 Terminal，并恢复默认高度
     */
    void showTerminalWithDefaultHeight();

    /**
     * @brief 折叠 Terminal
     */
    void collapseTerminal();

    /**
     * @brief 同步 ActivityBar 四个按钮状态
     */
    void updateActivityBarState();

    /**
     * @brief 判断 AI 面板当前是否可见
     * @return true 表示可见
     */
    bool isAiPanelVisible() const;

    /**
     * @brief 判断 Terminal 面板当前是否可见
     * @return true 表示可见
     */
    bool isTerminalPanelVisible() const;

private:
    MainWindow* mainWindow;                    ///< 主窗口引用
    ActivityBarWidget* activityBarWidget;      ///< 最左侧 ActivityBar
    SidePanelContainer* sidePanelContainer;    ///< 左侧面板容器（File/Log）
    EditorWidget* editorWidget;                ///< 中央编辑器
    TableWidget* tableWidget;                  ///< 表格组件
    TerminalWidget* terminalWidget;            ///< 底部终端
    AiPanelWidget* aiPanelWidget;              ///< 右侧 AI 面板
    QStackedWidget* mainDisplayStackedWidget;  ///< 中央主显示区（Editor/Table）
    QSplitter* topHorizontalSplitter;          ///< 上方水平分割：SidePanel/Main/AI
    QSplitter* rootVerticalSplitter;           ///< 右侧垂直分割：上方主工作区 + 下方Terminal
};

