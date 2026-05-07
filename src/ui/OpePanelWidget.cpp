/**
 * @file OpePanelWidget.cpp
 * @author YuzhSong
 * @brief 操作工作区主容器实现文件
 * @details 仅实现布局组织和面板显隐协调，不改动既有编辑器/目录/终端核心业务逻辑
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

#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>
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
    , aiPanelExpandWidth(260)
    , terminalExpandHeight(190)
{
    initUI();
    initStyle();
    initConnections();
}

/**
 * @brief 初始化界面布局
 * @details
 * 1. 上层使用水平 Splitter 管理 ActivityBar / SidePanel / MainDisplay / AiPanel。
 * 2. 根布局使用垂直 Splitter 管理“上层工作区”与“下层 Terminal”。
 * 3. 通过 setSizes 控制初始展开比例，避免覆盖式面板。
 */
void OpePanelWidget::initUI()
{
    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    // 作者：YuzhSong
    // SidePanel 允许宽度压缩到 0，保证能够被完全收起。
    sidePanelContainer->setMinimumWidth(0);
    aiPanelWidget->setMinimumWidth(0);
    aiPanelWidget->setMaximumWidth(QWIDGETSIZE_MAX);

    topHorizontalSplitter->addWidget(activityBarWidget);
    topHorizontalSplitter->addWidget(sidePanelContainer);
    topHorizontalSplitter->addWidget(mainDisplayStackedWidget);
    topHorizontalSplitter->addWidget(aiPanelWidget);
    topHorizontalSplitter->setCollapsible(0, false);
    topHorizontalSplitter->setCollapsible(1, true);
    topHorizontalSplitter->setCollapsible(2, false);
    topHorizontalSplitter->setCollapsible(3, true);
    topHorizontalSplitter->setStretchFactor(0, 0);
    topHorizontalSplitter->setStretchFactor(1, 0);
    topHorizontalSplitter->setStretchFactor(2, 1);
    topHorizontalSplitter->setStretchFactor(3, 0);
    topHorizontalSplitter->setHandleWidth(4);
    topHorizontalSplitter->setSizes({48, 220, 1100, 0});

    rootVerticalSplitter->addWidget(topHorizontalSplitter);
    rootVerticalSplitter->addWidget(terminalWidget);
    rootVerticalSplitter->setCollapsible(0, false);
    rootVerticalSplitter->setCollapsible(1, true);
    rootVerticalSplitter->setStretchFactor(0, 1);
    rootVerticalSplitter->setStretchFactor(1, 0);
    rootVerticalSplitter->setHandleWidth(4);
    rootVerticalSplitter->setSizes({900, 0});

    auto* mainLayout = new QVBoxLayout(this);
    // 作者：YuzhSong
    // 主工作区外围增加轻微留白，增强各面板分层；不改变 splitter 分配逻辑。
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(rootVerticalSplitter, 1);

    // 默认右侧 AI 面板与底部 Terminal 都收起。
    hideAiPanel();
    hideTerminalPanel();
    updateActivityBarState();
}

/**
 * @brief 初始化容器样式
 * @details 维持深灰基调，避免破坏原有 Run 按钮或业务组件自有样式
 */
void OpePanelWidget::initStyle()
{
    // 作者：YuzhSong
    // 统一工作区底色与 splitter 把手样式，保持深灰主调与轻微赛博感细节。
    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1F22;"
        "    color: #F0F0F0;"
        "}"
        "QSplitter::handle {"
        "    background-color: #3A3D42;"
        "    border-radius: 2px;"
        "}"
        "QSplitter::handle:hover {"
        "    background-color: #4A4E55;"
        "}"
    ));
}

/**
 * @brief 初始化信号槽连接
 * @details
 * 1. 保留 DirectoryWidget 与 EditorWidget 的既有联动。
 * 2. 保留 Editor 与 Terminal 的执行链路，不改动业务逻辑。
 * 3. 将 ActivityBar 的四个按钮请求映射为对应面板显隐操作。
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

    connect(activityBarWidget, &ActivityBarWidget::filePanelRequested, this, [this]() {
        sidePanelContainer->toggleFilePanel();
        updateActivityBarState();
    });

    connect(activityBarWidget, &ActivityBarWidget::logPanelRequested, this, [this]() {
        sidePanelContainer->toggleLogPanel();
        updateActivityBarState();
    });

    connect(activityBarWidget, &ActivityBarWidget::aiPanelRequested,
            this, &OpePanelWidget::toggleAiPanel);
    connect(activityBarWidget, &ActivityBarWidget::terminalPanelRequested,
            this, &OpePanelWidget::toggleTerminalPanel);
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
    sidePanelContainer->toggleFilePanel();
    updateActivityBarState();
}

void OpePanelWidget::collapseDirectoryPanel()
{
    sidePanelContainer->collapsePanel();
    updateActivityBarState();
}

void OpePanelWidget::expandDirectoryPanel()
{
    sidePanelContainer->showFilePanel();
    updateActivityBarState();
}

/**
 * @brief 切换 AI 面板显示状态
 * @details 根据当前可见状态在 show/hide 之间切换，并同步按钮高亮
 */
void OpePanelWidget::toggleAiPanel()
{
    if (isAiPanelVisible()) {
        hideAiPanel();
    } else {
        showAiPanel();
    }
    updateActivityBarState();
}

/**
 * @brief 展开 AI 面板
 * @details
 * 1. 仅修改 splitter 尺寸，不覆盖 EditorWidget。
 * 2. 通过压缩中央区域宽度实现“并排显示”。
 */
void OpePanelWidget::showAiPanel()
{
    aiPanelWidget->setVisible(true);

    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() != 4) {
        topHorizontalSplitter->setSizes({48, 220, 1000, aiPanelExpandWidth});
        return;
    }

    const int currentAiWidth = sizes[3];
    if (currentAiWidth <= 0) {
        const int delta = aiPanelExpandWidth;
        sizes[3] = aiPanelExpandWidth;
        sizes[2] = qMax(240, sizes[2] - delta);
    } else {
        sizes[3] = aiPanelExpandWidth;
    }
    topHorizontalSplitter->setSizes(sizes);
}

/**
 * @brief 收起 AI 面板
 * @details 保留组件实例，仅将宽度压缩为 0，便于后续快速展开
 */
void OpePanelWidget::hideAiPanel()
{
    QList<int> sizes = topHorizontalSplitter->sizes();
    if (sizes.size() == 4) {
        sizes[2] += sizes[3];
        sizes[3] = 0;
        topHorizontalSplitter->setSizes(sizes);
    }
    aiPanelWidget->setVisible(false);
}

/**
 * @brief 切换 Terminal 面板显示状态
 * @details 根据当前可见状态在 show/hide 之间切换，并同步按钮高亮
 */
void OpePanelWidget::toggleTerminalPanel()
{
    if (isTerminalPanelVisible()) {
        hideTerminalPanel();
    } else {
        showTerminalPanel();
    }
    updateActivityBarState();
}

/**
 * @brief 展开 Terminal 面板
 * @details 使用垂直 splitter 压缩上方工作区高度，不改动 Terminal 内部逻辑
 */
void OpePanelWidget::showTerminalPanel()
{
    terminalWidget->setVisible(true);

    QList<int> sizes = rootVerticalSplitter->sizes();
    if (sizes.size() != 2) {
        rootVerticalSplitter->setSizes({700, terminalExpandHeight});
        return;
    }

    if (sizes[1] <= 0) {
        const int delta = terminalExpandHeight;
        sizes[1] = terminalExpandHeight;
        sizes[0] = qMax(260, sizes[0] - delta);
    } else {
        sizes[1] = terminalExpandHeight;
    }
    rootVerticalSplitter->setSizes(sizes);
}

/**
 * @brief 收起 Terminal 面板
 * @details 保留终端组件实例，仅将分割高度压缩为 0
 */
void OpePanelWidget::hideTerminalPanel()
{
    QList<int> sizes = rootVerticalSplitter->sizes();
    if (sizes.size() == 2) {
        sizes[0] += sizes[1];
        sizes[1] = 0;
        rootVerticalSplitter->setSizes(sizes);
    }
    terminalWidget->setVisible(false);
}

/**
 * @brief 同步 ActivityBar 按钮状态
 * @details
 * 1. File/Log 状态基于 SidePanelContainer 当前可见面板类型。
 * 2. AI/Terminal 状态基于对应组件显隐状态。
 */
void OpePanelWidget::updateActivityBarState()
{
    const bool sidePanelVisible = sidePanelContainer->isPanelVisible();
    const auto panelType = sidePanelContainer->currentPanelType();

    const bool fileActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::File;
    const bool logActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::Log;
    const bool aiActive = isAiPanelVisible();
    const bool terminalActive = isTerminalPanelVisible();

    activityBarWidget->updateSelectionState(fileActive, logActive, aiActive, terminalActive);
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
