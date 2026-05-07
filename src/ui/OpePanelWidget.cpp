/**
 * @file OpePanelWidget.cpp
 * @author YuzhSong
 * @brief 操作工作区主容器实现文件
 * @details 仅实现布局组织与面板显隐协调，不改动编辑器、目录、终端等核心业务逻辑
 * @module ui
 */

#include "OpePanelWidget.h"

#include "opePanel/ActivityBarWidget.h"
#include "opePanel/AiPanelWidget.h"
#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/SidePanelContainer.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QStackedWidget>
#include <QtGlobal>

OpePanelWidget::OpePanelWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , activityBarWidget(new ActivityBarWidget(this))
    , sidePanelContainer(new SidePanelContainer(mainWindow, this))
    , editorWidget(new EditorWidget(mainWindow, this))
    , tableWidget(new TableWidget(mainWindow, this))
    , terminalWidget(new TerminalWidget(mainWindow, this))
    , aiPanelWidget(new AiPanelWidget(this))
    , mainDisplayStackedWidget(new QStackedWidget(this))
    , topHorizontalSplitter(new QSplitter(Qt::Horizontal, this))
    , rootVerticalSplitter(new QSplitter(Qt::Vertical, this))
{
    initUI();
    initStyle();
    initConnections();
}

/**
 * @brief 初始化界面布局
 * @details
 * 1. 将 ActivityBar 固定在工作区最左侧，不参与 Terminal 的上下分割。
 * 2. 右侧工作区采用“上方水平分割 + 下方 Terminal”结构，保证 Terminal 只占右侧底部。
 * 3. SidePanel / AI / Terminal 默认均为可折叠，展开时使用固定默认尺寸恢复。
 */
void OpePanelWidget::initUI()
{
    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    // 作者：YuzhSong
    // 允许面板尺寸压缩到 0，确保 QSplitter 折叠时可以完全收起。
    sidePanelContainer->setMinimumWidth(0);
    aiPanelWidget->setMinimumWidth(0);
    terminalWidget->setMinimumHeight(0);

    // 作者：YuzhSong
    // 上方主工作区仅包含 SidePanel / Editor / AI，避免 ActivityBar 被底部 Terminal 影响。
    topHorizontalSplitter->addWidget(sidePanelContainer);
    topHorizontalSplitter->addWidget(mainDisplayStackedWidget);
    topHorizontalSplitter->addWidget(aiPanelWidget);
    topHorizontalSplitter->setCollapsible(0, true);
    topHorizontalSplitter->setCollapsible(1, false);
    topHorizontalSplitter->setCollapsible(2, true);
    topHorizontalSplitter->setStretchFactor(0, 0);
    topHorizontalSplitter->setStretchFactor(1, 1);
    topHorizontalSplitter->setStretchFactor(2, 0);
    topHorizontalSplitter->setHandleWidth(WORKSPACE_GAP);
    topHorizontalSplitter->setSizes({SIDE_PANEL_DEFAULT_WIDTH, 1200, AI_PANEL_DEFAULT_WIDTH});

    // 作者：YuzhSong
    // 右侧工作区采用垂直分割：上方主工作区 + 下方 Terminal。
    rootVerticalSplitter->addWidget(topHorizontalSplitter);
    rootVerticalSplitter->addWidget(terminalWidget);
    rootVerticalSplitter->setCollapsible(0, false);
    rootVerticalSplitter->setCollapsible(1, true);
    rootVerticalSplitter->setStretchFactor(0, 1);
    rootVerticalSplitter->setStretchFactor(1, 0);
    rootVerticalSplitter->setHandleWidth(WORKSPACE_GAP);
    rootVerticalSplitter->setSizes({900, 0});

    // 作者：YuzhSong
    // 外层改为水平布局：左侧固定 ActivityBar，右侧完整工作区。
    auto* mainLayout = new QHBoxLayout(this);
    // 作者：YuzhSong
    // 主工作区使用 4px 统一缝隙，配合父容器 #363636 形成 IDE 区域分隔，不依赖额外假分隔线控件。
    // 作者：YuzhSong
    // 主容器外边距统一为 8px，提升面板边缘规整度并对齐上下区域。
    mainLayout->setContentsMargins(WORKSPACE_MARGIN, WORKSPACE_MARGIN, WORKSPACE_MARGIN, WORKSPACE_MARGIN);
    mainLayout->setSpacing(WORKSPACE_GAP);

    activityBarWidget->setFixedWidth(ACTIVITY_BAR_WIDTH);
    mainLayout->addWidget(activityBarWidget, 0);
    mainLayout->addWidget(rootVerticalSplitter, 1);

    showAiPanelWithDefaultWidth();
    collapseTerminal();
    showSidePanelWithDefaultWidth();
    sidePanelContainer->showFilePanel();
    updateActivityBarState();
}

void OpePanelWidget::initStyle()
{
    // 作者：YuzhSong
    // 统一将容器底色设置为 #363636，布局间距区域自然呈现“缝隙分隔”效果；分割条保持 4px、低亮度深灰。
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #26282C;"
        "    color: #F0F0F0;"
        "}"
        "QSplitter::handle {"
        "    background-color: #26282C;"
        "    border-radius: 2px;"
        "}"
        "QSplitter::handle:hover {"
        "    background-color: #34373C;"
        "}"
    ));
}

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

    // 作者：YuzhSong
    // ActivityBar 仅发出“请求”信号，实际面板控制保持在 OpePanelWidget 内，避免组件间强耦合。
    connect(activityBarWidget, &ActivityBarWidget::filePanelRequested, this, &OpePanelWidget::toggleFilePanel);
    connect(activityBarWidget, &ActivityBarWidget::logPanelRequested, this, &OpePanelWidget::toggleLogPanel);
    connect(activityBarWidget, &ActivityBarWidget::aiPanelRequested, this, &OpePanelWidget::toggleAiPanel);
    connect(activityBarWidget, &ActivityBarWidget::terminalPanelRequested, this, &OpePanelWidget::toggleTerminalPanel);
}

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

void OpePanelWidget::toggleDirectoryPanel()
{
    toggleFilePanel();
}

void OpePanelWidget::collapseDirectoryPanel()
{
    collapseSidePanel();
    updateActivityBarState();
}

void OpePanelWidget::expandDirectoryPanel()
{
    sidePanelContainer->showFilePanel();
    showSidePanelWithDefaultWidth();
    updateActivityBarState();
}

void OpePanelWidget::toggleFilePanel()
{
    const bool samePanelVisible = sidePanelContainer->isPanelVisible()
                                  && sidePanelContainer->currentPanelType() == SidePanelContainer::PanelType::File;

    if (samePanelVisible) {
        collapseSidePanel();
    } else {
        sidePanelContainer->showFilePanel();
        showSidePanelWithDefaultWidth();
    }

    updateActivityBarState();
}

void OpePanelWidget::toggleLogPanel()
{
    const bool samePanelVisible = sidePanelContainer->isPanelVisible()
                                  && sidePanelContainer->currentPanelType() == SidePanelContainer::PanelType::Log;

    if (samePanelVisible) {
        collapseSidePanel();
    } else {
        sidePanelContainer->showLogPanel();
        showSidePanelWithDefaultWidth();
    }

    updateActivityBarState();
}

void OpePanelWidget::showSidePanelWithDefaultWidth()
{
    sidePanelContainer->setVisible(true);

    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() != 3) {
        topHorizontalSplitter->setSizes({SIDE_PANEL_DEFAULT_WIDTH, 1200, 0});
        return;
    }

    const int aiWidth = isAiPanelVisible() ? AI_PANEL_DEFAULT_WIDTH : 0;
    sizes[0] = SIDE_PANEL_DEFAULT_WIDTH;
    sizes[2] = aiWidth;
    sizes[1] = qMax(300, topHorizontalSplitter->width() - sizes[0] - sizes[2]);
    topHorizontalSplitter->setSizes(sizes);
}

void OpePanelWidget::collapseSidePanel()
{
    sidePanelContainer->collapsePanel();
    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() == 3) {
        sizes[1] += sizes[0];
        sizes[0] = 0;
        topHorizontalSplitter->setSizes(sizes);
    }
}

void OpePanelWidget::toggleAiPanel()
{
    if (isAiPanelVisible()) {
        collapseAiPanel();
    } else {
        showAiPanelWithDefaultWidth();
    }
    updateActivityBarState();
}

void OpePanelWidget::showAiPanelWithDefaultWidth()
{
    aiPanelWidget->setVisible(true);

    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() != 3) {
        topHorizontalSplitter->setSizes({SIDE_PANEL_DEFAULT_WIDTH, 1200, AI_PANEL_DEFAULT_WIDTH});
        return;
    }

    const int sideWidth = sidePanelContainer->isPanelVisible() ? SIDE_PANEL_DEFAULT_WIDTH : 0;
    sizes[0] = sideWidth;
    sizes[2] = AI_PANEL_DEFAULT_WIDTH;
    sizes[1] = qMax(300, topHorizontalSplitter->width() - sizes[0] - sizes[2]);
    topHorizontalSplitter->setSizes(sizes);
}

void OpePanelWidget::collapseAiPanel()
{
    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() == 3) {
        sizes[1] += sizes[2];
        sizes[2] = 0;
        topHorizontalSplitter->setSizes(sizes);
    }
    aiPanelWidget->setVisible(false);
}

void OpePanelWidget::toggleTerminalPanel()
{
    if (isTerminalPanelVisible()) {
        collapseTerminal();
    } else {
        showTerminalWithDefaultHeight();
    }
    updateActivityBarState();
}

void OpePanelWidget::showTerminalWithDefaultHeight()
{
    terminalWidget->setVisible(true);

    QList<int> sizes = rootVerticalSplitter->sizes();
    if (sizes.size() != 2) {
        rootVerticalSplitter->setSizes({700, TERMINAL_DEFAULT_HEIGHT});
        return;
    }

    sizes[1] = TERMINAL_DEFAULT_HEIGHT;
    sizes[0] = qMax(260, rootVerticalSplitter->height() - sizes[1]);
    rootVerticalSplitter->setSizes(sizes);
}

void OpePanelWidget::collapseTerminal()
{
    QList<int> sizes = rootVerticalSplitter->sizes();
    if (sizes.size() == 2) {
        sizes[0] += sizes[1];
        sizes[1] = 0;
        rootVerticalSplitter->setSizes(sizes);
    }
    terminalWidget->setVisible(false);
}

void OpePanelWidget::updateActivityBarState()
{
    const bool sidePanelVisible = sidePanelContainer->isPanelVisible();
    const auto panelType = sidePanelContainer->currentPanelType();

    const bool fileActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::File;
    const bool logActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::Log;
    const bool aiActive = isAiPanelVisible();
    const bool terminalActive = isTerminalPanelVisible();

    activityBarWidget->setFileChecked(fileActive);
    activityBarWidget->setLogChecked(logActive);
    activityBarWidget->setAiChecked(aiActive);
    activityBarWidget->setTerminalChecked(terminalActive);
}

bool OpePanelWidget::isAiPanelVisible() const
{
    return aiPanelWidget->isVisible() && aiPanelWidget->width() > 0;
}

bool OpePanelWidget::isTerminalPanelVisible() const
{
    return terminalWidget->isVisible() && terminalWidget->height() > 0;
}

MainWindow* OpePanelWidget::getMainWindow() const
{
    return mainWindow;
}

DirectoryWidget* OpePanelWidget::getDirectoryWidget() const
{
    return sidePanelContainer->directoryWidget();
}

EditorWidget* OpePanelWidget::getEditorWidget() const
{
    return editorWidget;
}

TableWidget* OpePanelWidget::getTableWidget() const
{
    return tableWidget;
}

TerminalWidget* OpePanelWidget::getTerminalWidget() const
{
    return terminalWidget;
}

QStackedWidget* OpePanelWidget::getMainDisplayStackedWidget() const
{
    return mainDisplayStackedWidget;
}

