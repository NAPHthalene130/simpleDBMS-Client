/**
 * @file OpePanelWidget.cpp
 * @brief 操作工作区主容器实现文件
 * @details 负责 IDE 风格工作区布局搭建与基础显示逻辑连接，不改动编辑器/终端核心业务。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui
 */

#include "OpePanelWidget.h"

#include "opePanel/ActivityBarWidget.h"
#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/SidePanelContainer.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param mainWindow 主窗口指针
 * @param parent 父组件指针
 * @author YuzhSong
 */
OpePanelWidget::OpePanelWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , activityBarWidget(new ActivityBarWidget(this))
    , sidePanelContainer(new SidePanelContainer(mainWindow, this))
    , editorWidget(new EditorWidget(mainWindow, this))
    , tableWidget(new TableWidget(mainWindow, this))
    , terminalWidget(new TerminalWidget(mainWindow, this))
    , mainDisplayStackedWidget(new QStackedWidget(this))
    , rightPanelWidget(new QWidget(this))
    , horizontalSplitter(new QSplitter(Qt::Horizontal, this))
{
    initUI();
    initConnections();
}

/**
 * @brief 初始化界面布局
 * @details
 * 1. 水平方向采用 ActivityBar + SidePanel + RightPanel 结构。
 * 2. RightPanel 内部继续保留“主显示区 + TerminalWidget”垂直结构，保证原有终端位置不变。
 * @author YuzhSong
 */
void OpePanelWidget::initUI()
{
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1E1E;"
        "    color: #D4D4D4;"
        "}"
        "QSplitter::handle {"
        "    background-color: #3E3E3E;"
        "}"
    ));

    auto* rightLayout = new QVBoxLayout(rightPanelWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    rightLayout->addWidget(mainDisplayStackedWidget, 1);
    rightLayout->addWidget(terminalWidget, 0);

    // SidePanelContainer 允许最小宽度为 0，后续可被完全收起。
    sidePanelContainer->setMinimumWidth(0);

    // 使用三段式 splitter：最左窄 ActivityBar，中间可收起 SidePanel，右侧主工作区。
    horizontalSplitter->addWidget(activityBarWidget);
    horizontalSplitter->addWidget(sidePanelContainer);
    horizontalSplitter->addWidget(rightPanelWidget);
    horizontalSplitter->setCollapsible(0, false);
    horizontalSplitter->setCollapsible(1, true);
    horizontalSplitter->setCollapsible(2, false);
    horizontalSplitter->setStretchFactor(0, 0);
    horizontalSplitter->setStretchFactor(1, 0);
    horizontalSplitter->setStretchFactor(2, 1);
    horizontalSplitter->setHandleWidth(4);
    horizontalSplitter->setSizes({48, 220, 1200});

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(horizontalSplitter, 1);

    syncActivityBarState();
}

/**
 * @brief 初始化信号槽连接
 * @details
 * 1. 保持 DirectoryWidget 与 EditorWidget 原有文件联动链路不变。
 * 2. 新增 ActivityBar 与 SidePanelContainer 的切换连接。
 * 3. AI/Terminal 按钮当前仅保留信号入口，不接入具体面板逻辑。
 * @author YuzhSong
 */
void OpePanelWidget::initConnections()
{
    DirectoryWidget* directoryWidget = sidePanelContainer->directoryWidget();

    connect(editorWidget, &EditorWidget::fileOpened,
            directoryWidget, &DirectoryWidget::addOpenedFile);

    connect(editorWidget, &EditorWidget::currentFileChanged,
            directoryWidget, &DirectoryWidget::setCurrentFile);

    connect(directoryWidget, &DirectoryWidget::fileActivated,
            editorWidget, &EditorWidget::switchToFile);

    connect(editorWidget, &EditorWidget::toggleDirectoryRequested,
            this, &OpePanelWidget::toggleDirectoryPanel);

    connect(editorWidget, &EditorWidget::sqlExecuted,
            terminalWidget, &TerminalWidget::appendCommand);

    connect(terminalWidget, &TerminalWidget::sqlSubmitted,
            editorWidget, &EditorWidget::executeSql);

    // ActivityBar 仅发出“请求”，由 OpePanelWidget 协调 SidePanelContainer 进行显示状态变更。
    connect(activityBarWidget, &ActivityBarWidget::filePanelRequested, this, [this]() {
        sidePanelContainer->toggleFilePanel();
        syncActivityBarState();
    });

    connect(activityBarWidget, &ActivityBarWidget::logPanelRequested, this, [this]() {
        sidePanelContainer->toggleLogPanel();
        syncActivityBarState();
    });

    // 预留信号：当前阶段不实现 AI / Terminal 左侧面板逻辑，仅维持按钮可点击与扩展点完整性。
    connect(activityBarWidget, &ActivityBarWidget::aiPanelRequested, this, [this]() {
        syncActivityBarState();
    });

    connect(activityBarWidget, &ActivityBarWidget::terminalPanelRequested, this, [this]() {
        syncActivityBarState();
    });
}

/**
 * @brief 同步 ActivityBar 选中态
 * @details 基于 SidePanelContainer 当前可见状态与面板类型刷新 File/Log 高亮。
 * @author YuzhSong
 */
void OpePanelWidget::syncActivityBarState()
{
    const bool panelVisible = sidePanelContainer->isPanelVisible();
    const auto panelType = sidePanelContainer->currentPanelType();

    const bool fileActive = panelVisible && panelType == SidePanelContainer::PanelType::File;
    const bool logActive = panelVisible && panelType == SidePanelContainer::PanelType::Log;
    activityBarWidget->updateSelectionState(fileActive, logActive);
}

/**
 * @brief 切换中央主显示区组件
 * @param widget 目标显示组件
 * @author YuzhSong
 */
void OpePanelWidget::switchWidget(QWidget* widget)
{
    if (widget == nullptr) {
        return;
    }

    const int widgetIndex = mainDisplayStackedWidget->indexOf(widget);
    if (widgetIndex < 0) {
        return;
    }

    mainDisplayStackedWidget->setCurrentIndex(widgetIndex);
}

/**
 * @brief 切换 File 面板显示状态（兼容旧入口）
 * @details 复用 SidePanelContainer 的 toggleFilePanel 逻辑。
 * @author YuzhSong
 */
void OpePanelWidget::toggleDirectoryPanel()
{
    sidePanelContainer->toggleFilePanel();
    syncActivityBarState();
}

/**
 * @brief 收起左侧宽面板（兼容旧入口）
 * @author YuzhSong
 */
void OpePanelWidget::collapseDirectoryPanel()
{
    sidePanelContainer->collapsePanel();
    syncActivityBarState();
}

/**
 * @brief 展开左侧宽面板并显示 File 面板（兼容旧入口）
 * @author YuzhSong
 */
void OpePanelWidget::expandDirectoryPanel()
{
    sidePanelContainer->showFilePanel();
    syncActivityBarState();
}

/**
 * @brief 获取主窗口指针
 * @return 主窗口指针
 * @author YuzhSong
 */
MainWindow* OpePanelWidget::getMainWindow() const
{
    return mainWindow;
}

/**
 * @brief 获取目录组件
 * @return 目录组件指针
 * @author YuzhSong
 */
DirectoryWidget* OpePanelWidget::getDirectoryWidget() const
{
    return sidePanelContainer->directoryWidget();
}

/**
 * @brief 获取编辑器组件
 * @return 编辑器组件指针
 * @author YuzhSong
 */
EditorWidget* OpePanelWidget::getEditorWidget() const
{
    return editorWidget;
}

/**
 * @brief 获取表格组件
 * @return 表格组件指针
 * @author YuzhSong
 */
TableWidget* OpePanelWidget::getTableWidget() const
{
    return tableWidget;
}

/**
 * @brief 获取终端组件
 * @return 终端组件指针
 * @author YuzhSong
 */
TerminalWidget* OpePanelWidget::getTerminalWidget() const
{
    return terminalWidget;
}

/**
 * @brief 获取中央主显示区容器
 * @return 中央主显示区容器指针
 * @author YuzhSong
 */
QStackedWidget* OpePanelWidget::getMainDisplayStackedWidget() const
{
    return mainDisplayStackedWidget;
}

