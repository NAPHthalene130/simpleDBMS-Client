/**
 * @file SidePanelContainer.cpp
 * @brief 左侧宽面板容器组件实现文件
 * @details 负责 DirectoryWidget / LogWidget 切换与面板收起逻辑，不承载业务逻辑。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#include "SidePanelContainer.h"

#include "DirectoryWidget.h"
#include "LogWidget.h"

#include <QSizePolicy>
#include <QStackedWidget>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param mainWindow 主窗口指针
 * @param parent 父组件指针
 * @author YuzhSong
 */
SidePanelContainer::SidePanelContainer(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , stackedWidget(nullptr)
    , m_directoryWidget(nullptr)
    , m_logWidget(nullptr)
    , m_currentPanelType(PanelType::File)
    , m_panelVisible(true)
{
    initUI();
    initStyle();
    showFilePanel();
}

/**
 * @brief 初始化界面结构
 * @details
 * 1. 使用 QStackedWidget 管理 File/Log 面板。
 * 2. 默认宽度为 IDE 常见侧栏宽度，且保留 setMinimumWidth(0) 以支持未来完全收起。
 * @author YuzhSong
 */
void SidePanelContainer::initUI()
{
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(0);

    stackedWidget = new QStackedWidget(this);
    m_directoryWidget = new DirectoryWidget(mainWindow, this);
    m_logWidget = new LogWidget(this);

    stackedWidget->addWidget(m_directoryWidget);
    stackedWidget->addWidget(m_logWidget);

    mainLayout->addWidget(stackedWidget, 1);
}

/**
 * @brief 初始化样式
 * @details 面板维持深灰风格，避免与编辑区产生突兀视觉差异。
 * @author YuzhSong
 */
void SidePanelContainer::initStyle()
{
    setStyleSheet(
        "SidePanelContainer {"
        "    background-color: #252A31;"
        "    border: 1px solid #383D44;"
        "    border-radius: 8px;"
        "}"
    );
}

/**
 * @brief 显示 File 面板
 * @details 切换到 DirectoryWidget，并确保容器处于可见状态。
 * @author YuzhSong
 */
void SidePanelContainer::showFilePanel()
{
    stackedWidget->setCurrentWidget(m_directoryWidget);
    m_currentPanelType = PanelType::File;
    m_panelVisible = true;
    setVisible(true);
}

/**
 * @brief 显示 Log 面板
 * @details 切换到 LogWidget，并确保容器处于可见状态。
 * @author YuzhSong
 */
void SidePanelContainer::showLogPanel()
{
    stackedWidget->setCurrentWidget(m_logWidget);
    m_currentPanelType = PanelType::Log;
    m_panelVisible = true;
    setVisible(true);
}

/**
 * @brief 切换 File 面板（支持重复点击收起）
 * @details
 * 1. 如果当前已显示且当前面板是 File，则收起容器。
 * 2. 否则显示 File 面板。
 * @author YuzhSong
 */
void SidePanelContainer::toggleFilePanel()
{
    if (m_panelVisible && m_currentPanelType == PanelType::File) {
        collapsePanel();
        return;
    }

    showFilePanel();
}

/**
 * @brief 切换 Log 面板（支持重复点击收起）
 * @details
 * 1. 如果当前已显示且当前面板是 Log，则收起容器。
 * 2. 否则显示 Log 面板。
 * @author YuzhSong
 */
void SidePanelContainer::toggleLogPanel()
{
    if (m_panelVisible && m_currentPanelType == PanelType::Log) {
        collapsePanel();
        return;
    }

    showLogPanel();
}

/**
 * @brief 收起面板容器
 * @details
 * 1. 仅控制 SidePanelContainer 自身显示状态。
 * 2. 不销毁内部组件，便于下次快速恢复。
 * 3. 将 currentPanelType 置为 None，表示当前无激活面板。
 * @author YuzhSong
 */
void SidePanelContainer::collapsePanel()
{
    m_panelVisible = false;
    m_currentPanelType = PanelType::None;
    setVisible(false);
}

/**
 * @brief 判断面板容器是否可见
 * @return 可见返回 true，否则返回 false
 * @author YuzhSong
 */
bool SidePanelContainer::isPanelVisible() const
{
    return m_panelVisible && isVisible();
}

/**
 * @brief 获取目录面板组件
 * @return DirectoryWidget 指针
 * @author YuzhSong
 */
DirectoryWidget* SidePanelContainer::directoryWidget() const
{
    return m_directoryWidget;
}

/**
 * @brief 获取日志面板组件
 * @return LogWidget 指针
 * @author YuzhSong
 */
LogWidget* SidePanelContainer::logWidget() const
{
    return m_logWidget;
}

/**
 * @brief 获取当前激活面板类型
 * @return 当前面板类型
 * @author YuzhSong
 */
SidePanelContainer::PanelType SidePanelContainer::currentPanelType() const
{
    return m_currentPanelType;
}
