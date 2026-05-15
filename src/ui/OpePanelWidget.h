/**
 * @file OpePanelWidget.h
 * @author YuzhSong
 * @brief 操作工作区主容器头文件
 * @details 负责协调 ActivityBar、SidePanel、Editor、AI、Terminal 的布局与显隐，不承载核心业务逻辑
 * @module ui
 */

#pragma once

#include <QWidget>
#include <vector>

#include "models/network/NetworkTransferData.h"

class MainWindow;
class QSplitter;
class QStackedWidget;
class DirectoryWidget;
class FileWidget;
class EditorWidget;
class TableWidget;
class TerminalWidget;
class ActivityBarWidget;
class SidePanelContainer;
class AiPanelWidget;

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

    /**
     * @brief 设置服务端目录数据
     * @author Qi
     * @param databases 服务端返回的数据库目录列表
     */
    void setDirectoryData(const std::vector<DatabaseNode> &databases);

    /**
     * @brief 获取缓存的目录数据
     * @author Qi
     * @return 数据库目录列表引用
     */
    const std::vector<DatabaseNode> &getDirectoryData() const;

    void refreshTheme();

public slots:
    void toggleDirectoryPanel();
    void collapseDirectoryPanel();
    void expandDirectoryPanel();

private:
    static constexpr int ACTIVITY_BAR_WIDTH = 35;
    static constexpr int SIDE_PANEL_DEFAULT_WIDTH = 255;
    static constexpr int AI_PANEL_DEFAULT_WIDTH = 330;
    static constexpr int TERMINAL_DEFAULT_HEIGHT = 220;
    static constexpr int WORKSPACE_GAP = 8;
    static constexpr int WORKSPACE_MARGIN = 8;

    void initUI();
    void initConnections();
    void initStyle();

    void toggleFilePanel();
    void toggleLogPanel();
    void toggleAiPanel();

    void showSidePanelWithDefaultWidth();
    void collapseSidePanel();

    void showAiPanelWithDefaultWidth();
    void collapseAiPanel();

    void toggleTerminalPanel();
    void showTerminalWithDefaultHeight();
    void collapseTerminal();

    void updateActivityBarState();

    bool isAiPanelVisible() const;
    bool isTerminalPanelVisible() const;

private:
    MainWindow* mainWindow;
    ActivityBarWidget* activityBarWidget;
    SidePanelContainer* sidePanelContainer;
    EditorWidget* editorWidget;
    TableWidget* tableWidget;
    TerminalWidget* terminalWidget;
    AiPanelWidget* aiPanelWidget;
    QStackedWidget* mainDisplayStackedWidget;
    QSplitter* topHorizontalSplitter;
    QSplitter* rootVerticalSplitter;
    std::vector<DatabaseNode> directoryData;
};

