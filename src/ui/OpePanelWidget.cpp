/**
 * @file OpePanelWidget.cpp
 * @author YuzhSong
 * @brief 操作工作区主容器实现文件
 * @details 仅实现布局协调与面板切换，不修改 SQL 编辑器、自动补全、高亮、执行、终端核心逻辑
 * @module ui
 */

#include "OpePanelWidget.h"

#include "opePanel/ActivityBarWidget.h"
#include "opePanel/AiPanelWidget.h"
#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/FileWidget.h"
#include "opePanel/SidePanelContainer.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStackedWidget>
#include "ThemeManager.h"

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

void OpePanelWidget::initUI()
{
    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    sidePanelContainer->setMinimumWidth(0);
    aiPanelWidget->setMinimumWidth(0);
    terminalWidget->setMinimumHeight(0);

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

    rootVerticalSplitter->addWidget(topHorizontalSplitter);
    rootVerticalSplitter->addWidget(terminalWidget);
    rootVerticalSplitter->setCollapsible(0, false);
    rootVerticalSplitter->setCollapsible(1, true);
    rootVerticalSplitter->setStretchFactor(0, 1);
    rootVerticalSplitter->setStretchFactor(1, 0);
    rootVerticalSplitter->setHandleWidth(WORKSPACE_GAP);
    rootVerticalSplitter->setSizes({900, 0});

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(WORKSPACE_MARGIN, WORKSPACE_MARGIN, WORKSPACE_MARGIN, WORKSPACE_MARGIN);
    mainLayout->setSpacing(WORKSPACE_GAP);

    activityBarWidget->setFixedWidth(ACTIVITY_BAR_WIDTH);
    mainLayout->addWidget(activityBarWidget, 0);
    mainLayout->addWidget(rootVerticalSplitter, 1);

    showAiPanelWithDefaultWidth();
    collapseTerminal();
    showSidePanelWithDefaultWidth();
    sidePanelContainer->showDirectoryPanel();
    updateActivityBarState();
}

void OpePanelWidget::initStyle()
{
    setStyleSheet(ThemeManager::opePanel());
}

void OpePanelWidget::initConnections()
{
    DirectoryWidget* directoryWidget = sidePanelContainer->directoryWidget();
    FileWidget* fileWidget = sidePanelContainer->fileWidget();

    // 作者：YuzhSong
    // FileWidget 承接“已打开文件列表”职责，与 EditorWidget 通过 OpePanelWidget 连接，保持组件解耦。
    connect(editorWidget, &EditorWidget::fileOpened, fileWidget, &FileWidget::addOpenedFile);
    connect(editorWidget, &EditorWidget::currentFileChanged, fileWidget, &FileWidget::setCurrentFile);
    connect(editorWidget, &EditorWidget::fileModifiedStateChanged, fileWidget, &FileWidget::updateFileModifiedState);
    connect(editorWidget, &EditorWidget::fileClosed, fileWidget, &FileWidget::removeOpenedFile);
    connect(editorWidget, &EditorWidget::fileKeyChanged, fileWidget, &FileWidget::updateFileKey);
    connect(fileWidget, &FileWidget::fileActivated, editorWidget, &EditorWidget::switchToFile);
    connect(fileWidget, &FileWidget::closeFileRequested, editorWidget, &EditorWidget::closeFile);

    connect(editorWidget, &EditorWidget::toggleDirectoryRequested, this, &OpePanelWidget::toggleDirectoryPanel);
    connect(editorWidget, &EditorWidget::sqlExecuted, terminalWidget, &TerminalWidget::appendCommand);
    connect(terminalWidget, &TerminalWidget::sqlSubmitted, editorWidget, &EditorWidget::executeSql);

    connect(tableWidget, &TableWidget::backToEditorRequested, this, [this]() {
        mainDisplayStackedWidget->setCurrentWidget(editorWidget);
    });

    connect(activityBarWidget, &ActivityBarWidget::directoryPanelRequested, this, &OpePanelWidget::toggleDirectoryPanel);
    connect(activityBarWidget, &ActivityBarWidget::filePanelRequested, this, &OpePanelWidget::toggleFilePanel);
    connect(activityBarWidget, &ActivityBarWidget::logPanelRequested, this, &OpePanelWidget::toggleLogPanel);
    connect(activityBarWidget, &ActivityBarWidget::aiPanelRequested, this, &OpePanelWidget::toggleAiPanel);
    connect(activityBarWidget, &ActivityBarWidget::terminalPanelRequested, this, &OpePanelWidget::toggleTerminalPanel);

    // 作者：YuzhSong
    // DirectoryWidget 的双击信号当前只做接口预留，后续接入 SQL 生成/表结构查看功能。
    connect(directoryWidget, &DirectoryWidget::tableActivated, this, [](const QString& db, const QString& table) {
        qDebug() << "[DirectoryWidget] tableActivated:" << db << table;
    });
    connect(directoryWidget, &DirectoryWidget::columnActivated, this, [](const QString& db, const QString& table, const QString& column) {
        qDebug() << "[DirectoryWidget] columnActivated:" << db << table << column;
    });

    // 作者：YuzhSong
    // 连接目录刷新请求信号：当前阶段只做日志占位，后续这里可转发到服务端通信模块请求最新元数据。
    // 边界说明：该连接不改动 SQL 编辑、文件管理、终端输出等既有核心业务逻辑。
    connect(directoryWidget, &DirectoryWidget::refreshRequested, this, []() {
        qDebug() << "收到数据库目录刷新请求，后续将转发给服务端模块。";
    });
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
    sidePanelContainer->toggleDirectoryPanel();
    if (sidePanelContainer->isPanelVisible()) {
        showSidePanelWithDefaultWidth();
    } else {
        collapseSidePanel();
    }
    updateActivityBarState();
}

void OpePanelWidget::collapseDirectoryPanel()
{
    collapseSidePanel();
    updateActivityBarState();
}

void OpePanelWidget::expandDirectoryPanel()
{
    sidePanelContainer->showDirectoryPanel();
    showSidePanelWithDefaultWidth();
    updateActivityBarState();
}

void OpePanelWidget::toggleFilePanel()
{
    sidePanelContainer->toggleFilePanel();
    if (sidePanelContainer->isPanelVisible()) {
        showSidePanelWithDefaultWidth();
    } else {
        collapseSidePanel();
    }
    updateActivityBarState();
}

void OpePanelWidget::toggleLogPanel()
{
    sidePanelContainer->toggleLogPanel();
    if (sidePanelContainer->isPanelVisible()) {
        showSidePanelWithDefaultWidth();
    } else {
        collapseSidePanel();
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

    // 作者：YuzhSong
    // Directory/File/Log 三个按钮高亮互斥；AI 与 Terminal 独立高亮。
    const bool directoryActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::Directory;
    const bool fileActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::File;
    const bool logActive = sidePanelVisible && panelType == SidePanelContainer::PanelType::Log;

    activityBarWidget->setDirectoryChecked(directoryActive);
    activityBarWidget->setFileChecked(fileActive);
    activityBarWidget->setLogChecked(logActive);
    activityBarWidget->setAiChecked(isAiPanelVisible());
    activityBarWidget->setTerminalChecked(isTerminalPanelVisible());
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

void OpePanelWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::opePanel());
    if (editorWidget) editorWidget->refreshTheme();
    if (tableWidget) tableWidget->refreshTheme();
    if (terminalWidget) terminalWidget->refreshTheme();
    if (activityBarWidget) activityBarWidget->refreshTheme();
    if (sidePanelContainer) sidePanelContainer->refreshTheme();
    if (aiPanelWidget) aiPanelWidget->refreshTheme();
}
