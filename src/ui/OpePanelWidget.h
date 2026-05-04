/**
 * @file OpePanelWidget.h
 * @brief 操作面板容器组件头文件
 * @details 负责协调左侧 DirectoryWidget 与右侧主显示区（Editor/Table + Terminal）的布局和信号交互。
 *          该类作为协调层，统一处理侧边栏展开/收起，避免 DirectoryWidget 与 EditorWidget 相互直接依赖。
 * @author YuzhSong
 * @date 2026-04-26
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

/**
 * @class OpePanelWidget
 * @brief 操作面板页面组件
 * @details 用户登录后的主操作区域，包含：
 *          1. 左侧 DirectoryWidget（可折叠）
 *          2. 右侧主显示区（EditorWidget / TableWidget 切换）
 *          3. 下方 TerminalWidget
 * @author YuzhSong
 */
class OpePanelWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param mainWindow 主窗口指针
     * @param parent 父组件指针
     */
    explicit OpePanelWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    /**
     * @brief 切换中央主显示区组件
     * @author YuzhSong
     * @param widget 目标显示组件
     */
    void switchWidget(QWidget* widget);

    /**
     * @brief 获取主窗口指针
     * @author YuzhSong
     * @return 主窗口指针
     */
    MainWindow* getMainWindow() const;

    /**
     * @brief 获取左侧目录组件
     * @author YuzhSong
     * @return 目录组件指针
     */
    DirectoryWidget* getDirectoryWidget() const;

    /**
     * @brief 获取中央编辑器组件
     * @author YuzhSong
     * @return 编辑器组件指针
     */
    EditorWidget* getEditorWidget() const;

    /**
     * @brief 获取中央数据表格组件
     * @author YuzhSong
     * @return 数据表格组件指针
     */
    TableWidget* getTableWidget() const;

    /**
     * @brief 获取下侧终端组件
     * @author YuzhSong
     * @return 终端组件指针
     */
    TerminalWidget* getTerminalWidget() const;

    /**
     * @brief 获取中央主显示区容器
     * @author YuzhSong
     * @return 主显示区容器指针
     */
    QStackedWidget* getMainDisplayStackedWidget() const;

public slots:
    /**
     * @brief 切换目录侧栏展开/收起
     * @author YuzhSong
     */
    void toggleDirectoryPanel();

    /**
     * @brief 收起目录侧栏
     * @author YuzhSong
     */
    void collapseDirectoryPanel();

    /**
     * @brief 展开目录侧栏
     * @author YuzhSong
     */
    void expandDirectoryPanel();

private:
    /**
     * @brief 初始化 UI 布局
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     * @author YuzhSong
     */
    void initConnections();

private:
    MainWindow* mainWindow;                  ///< 主窗口引用
    DirectoryWidget* directoryWidget;        ///< 左侧目录组件
    EditorWidget* editorWidget;              ///< 中央编辑器组件
    TableWidget* tableWidget;                ///< 中央数据表格组件
    TerminalWidget* terminalWidget;          ///< 下侧终端组件
    QStackedWidget* mainDisplayStackedWidget;///< 中央主显示区容器
    QWidget* rightPanelWidget;               ///< 右侧容器（中央区 + 终端）
    QSplitter* horizontalSplitter;           ///< 左右分割器
    bool directoryCollapsed;                 ///< 目录侧栏是否收起
    int lastDirectoryPanelWidth;             ///< 最近一次展开宽度，用于恢复
};

