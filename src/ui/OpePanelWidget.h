/**
 * @file OpePanelWidget.h
 * @author YuzhSong
 * @brief 操作工作区主容器头文件
 * @details 负责协调 ActivityBar、SidePanel、Editor、AI、Terminal 的布局与显隐，不承载核心业务逻辑
 * @module ui
 */

#pragma once

#include <QWidget>

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
};

