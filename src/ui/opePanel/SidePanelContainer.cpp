/**
 * @file SidePanelContainer.cpp
 * @author YuzhSong
 * @brief 左侧宽面板容器实现文件
 * @details 通过 QStackedWidget 管理 Directory/File/Log 三个互斥页面
 * @module ui/opePanel
 */

#include "SidePanelContainer.h"
#include "ui/ThemeManager.h"

#include "DirectoryWidget.h"
#include "FileWidget.h"
#include "LogWidget.h"

#include <QSizePolicy>
#include <QStackedWidget>
#include <QVBoxLayout>

SidePanelContainer::SidePanelContainer(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , stackedWidget(nullptr)
    , m_directoryWidget(nullptr)
    , m_fileWidget(nullptr)
    , m_logWidget(nullptr)
    , m_currentPanelType(PanelType::Directory)
    , m_panelVisible(true)
{
    initUI();
    initStyle();
    showDirectoryPanel();
}

void SidePanelContainer::initUI()
{
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 作者：YuzhSong
    // QStackedWidget 是管理互斥面板的最直接方案，同一时刻只显示一个页面，符合本任务要求。
    stackedWidget = new QStackedWidget(this);
    m_directoryWidget = new DirectoryWidget(this);
    m_fileWidget = new FileWidget(this);
    m_logWidget = new LogWidget(this);

    stackedWidget->addWidget(m_directoryWidget);
    stackedWidget->addWidget(m_fileWidget);
    stackedWidget->addWidget(m_logWidget);

    mainLayout->addWidget(stackedWidget, 1);
}

void SidePanelContainer::initStyle()
{
    setStyleSheet(
        "SidePanelContainer {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
    );
}

void SidePanelContainer::showDirectoryPanel()
{
    stackedWidget->setCurrentWidget(m_directoryWidget);
    m_currentPanelType = PanelType::Directory;
    m_panelVisible = true;
    setVisible(true);
}

void SidePanelContainer::showFilePanel()
{
    stackedWidget->setCurrentWidget(m_fileWidget);
    m_currentPanelType = PanelType::File;
    m_panelVisible = true;
    setVisible(true);
}

void SidePanelContainer::showLogPanel()
{
    stackedWidget->setCurrentWidget(m_logWidget);
    m_currentPanelType = PanelType::Log;
    m_panelVisible = true;
    setVisible(true);
}

void SidePanelContainer::toggleDirectoryPanel()
{
    if (m_panelVisible && m_currentPanelType == PanelType::Directory) {
        collapsePanel();
        return;
    }
    showDirectoryPanel();
}

void SidePanelContainer::toggleFilePanel()
{
    if (m_panelVisible && m_currentPanelType == PanelType::File) {
        collapsePanel();
        return;
    }
    showFilePanel();
}

void SidePanelContainer::toggleLogPanel()
{
    if (m_panelVisible && m_currentPanelType == PanelType::Log) {
        collapsePanel();
        return;
    }
    showLogPanel();
}

void SidePanelContainer::collapsePanel()
{
    m_panelVisible = false;
    m_currentPanelType = PanelType::None;
    setVisible(false);
}

bool SidePanelContainer::isPanelVisible() const
{
    return m_panelVisible && isVisible();
}

DirectoryWidget* SidePanelContainer::directoryWidget() const
{
    return m_directoryWidget;
}

FileWidget* SidePanelContainer::fileWidget() const
{
    return m_fileWidget;
}

LogWidget* SidePanelContainer::logWidget() const
{
    return m_logWidget;
}

SidePanelContainer::PanelType SidePanelContainer::currentPanelType() const
{
    return m_currentPanelType;
}

void SidePanelContainer::refreshTheme()
{
    setStyleSheet("SidePanelContainer { background-color: transparent; border: none; }");
    if (directoryWidget()) directoryWidget()->refreshTheme();
    if (fileWidget()) fileWidget()->refreshTheme();
    if (logWidget()) logWidget()->refreshTheme();
}

