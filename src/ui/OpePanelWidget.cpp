/**
 * @file OpePanelWidget.cpp
 * @brief 操作面板容器组件实现文件
 * @details 负责：
 *          1. 用 QSplitter 组织左侧 DirectoryWidget 与右侧主工作区
 *          2. 连接 DirectoryWidget 与 EditorWidget 的文件激活/同步信号
 *          3. 统一处理目录侧栏展开/收起逻辑
 * @author YuzhSong
 * @date 2026-04-26
 */

#include "OpePanelWidget.h"

#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QSplitter>
#include <QStackedWidget>
#include <QtGlobal>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param mainWindow 主窗口指针
 * @param parent 父组件指针
 */
OpePanelWidget::OpePanelWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , directoryWidget(new DirectoryWidget(mainWindow, this))
    , editorWidget(new EditorWidget(mainWindow, this))
    , tableWidget(new TableWidget(mainWindow, this))
    , terminalWidget(new TerminalWidget(mainWindow, this))
    , mainDisplayStackedWidget(new QStackedWidget(this))
    , rightPanelWidget(new QWidget(this))
    , horizontalSplitter(new QSplitter(Qt::Horizontal, this))
    , directoryCollapsed(false)
    , lastDirectoryPanelWidth(220)
{
    initUI();
    initConnections();
}

/**
 * @brief 初始化 UI 布局
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

    // 目录组件必须允许最小宽度为 0，才能在 splitter 中完全收起。
    directoryWidget->setMinimumWidth(0);

    // 外层采用 QSplitter，替代普通 QHBoxLayout 固定分配，提供 IDE 风格拖拽与折叠。
    horizontalSplitter->addWidget(directoryWidget);
    horizontalSplitter->addWidget(rightPanelWidget);
    horizontalSplitter->setCollapsible(0, true);
    horizontalSplitter->setCollapsible(1, false);
    horizontalSplitter->setStretchFactor(0, 0);
    horizontalSplitter->setStretchFactor(1, 1);
    horizontalSplitter->setHandleWidth(4);
    horizontalSplitter->setSizes({lastDirectoryPanelWidth, 1200});

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(horizontalSplitter, 1);
}

/**
 * @brief 初始化信号槽连接
 * @author YuzhSong
 */
void OpePanelWidget::initConnections()
{
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

    connect(horizontalSplitter, &QSplitter::splitterMoved, this, [this](int pos, int) {
        if (!directoryCollapsed && pos > 0) {
            lastDirectoryPanelWidth = qMax(120, pos);
        }
    });
}

/**
 * @brief 切换中央主显示区组件
 * @author YuzhSong
 * @param widget 目标显示组件
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
 * @brief 切换目录侧栏展开/收起
 * @author YuzhSong
 */
void OpePanelWidget::toggleDirectoryPanel()
{
    if (directoryCollapsed) {
        expandDirectoryPanel();
    } else {
        collapseDirectoryPanel();
    }
}

/**
 * @brief 收起目录侧栏
 * @author YuzhSong
 */
void OpePanelWidget::collapseDirectoryPanel()
{
    if (directoryCollapsed) {
        return;
    }

    const QList<int> sizes = horizontalSplitter->sizes();
    if (sizes.size() >= 2 && sizes[0] > 0) {
        lastDirectoryPanelWidth = qMax(120, sizes[0]);
    }

    directoryCollapsed = true;
    horizontalSplitter->setSizes({0, qMax(1, width())});
}

/**
 * @brief 展开目录侧栏
 * @author YuzhSong
 */
void OpePanelWidget::expandDirectoryPanel()
{
    if (!directoryCollapsed) {
        return;
    }

    directoryCollapsed = false;

    const QList<int> sizes = horizontalSplitter->sizes();
    int totalWidth = 0;
    for (const int size : sizes) {
        totalWidth += size;
    }

    if (totalWidth <= 0) {
        totalWidth = qMax(width(), 1000);
    }

    const int leftWidth = qBound(120, lastDirectoryPanelWidth, qMax(120, totalWidth - 300));
    const int rightWidth = qMax(300, totalWidth - leftWidth);
    horizontalSplitter->setSizes({leftWidth, rightWidth});
}

/**
 * @brief 获取主窗口指针
 * @author YuzhSong
 * @return 主窗口指针
 */
MainWindow* OpePanelWidget::getMainWindow() const
{
    return mainWindow;
}

/**
 * @brief 获取左侧目录组件
 * @author YuzhSong
 * @return 目录组件指针
 */
DirectoryWidget* OpePanelWidget::getDirectoryWidget() const
{
    return directoryWidget;
}

/**
 * @brief 获取中央编辑器组件
 * @author YuzhSong
 * @return 编辑器组件指针
 */
EditorWidget* OpePanelWidget::getEditorWidget() const
{
    return editorWidget;
}

/**
 * @brief 获取中央数据表格组件
 * @author YuzhSong
 * @return 数据表格组件指针
 */
TableWidget* OpePanelWidget::getTableWidget() const
{
    return tableWidget;
}

/**
 * @brief 获取下侧终端组件
 * @author YuzhSong
 * @return 终端组件指针
 */
TerminalWidget* OpePanelWidget::getTerminalWidget() const
{
    return terminalWidget;
}

/**
 * @brief 获取中央主显示区容器
 * @author YuzhSong
 * @return 主显示区容器指针
 */
QStackedWidget* OpePanelWidget::getMainDisplayStackedWidget() const
{
    return mainDisplayStackedWidget;
}
