/**
 * @file ActivityBarWidget.cpp
 * @author YuzhSong
 * @brief IDE 左侧 ActivityBar 实现文件
 * @details 仅负责按钮展示与请求信号发出，不负责具体面板布局控制
 * @module ui/opePanel
 */

#include "ActivityBarWidget.h"
#include "ui/ThemeManager.h"

#include <QPushButton>
#include <QVBoxLayout>

ActivityBarWidget::ActivityBarWidget(QWidget* parent)
    : QWidget(parent)
    , directoryButton(nullptr)
    , fileButton(nullptr)
    , logButton(nullptr)
    , aiButton(nullptr)
    , terminalButton(nullptr)
{
    initUI();
    initStyle();
    initConnections();
}

void ActivityBarWidget::initUI()
{
    setFixedWidth(35);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 8, 2, 8);
    mainLayout->setSpacing(8);

    directoryButton = new QPushButton(tr("D"), this);
    fileButton = new QPushButton(tr("F"), this);
    logButton = new QPushButton(tr("L"), this);
    aiButton = new QPushButton(tr("A"), this);
    terminalButton = new QPushButton(tr("T"), this);

    directoryButton->setToolTip(tr("Directory"));
    fileButton->setToolTip(tr("File"));
    logButton->setToolTip(tr("Log"));
    aiButton->setToolTip(tr("AI"));
    terminalButton->setToolTip(tr("Terminal"));

    const QList<QPushButton*> allButtons = {directoryButton, fileButton, logButton, aiButton, terminalButton};
    for (QPushButton* button : allButtons) {
        button->setCheckable(true);
        button->setFixedSize(30, 30);
    }

    mainLayout->addWidget(directoryButton);
    mainLayout->addWidget(fileButton);
    mainLayout->addWidget(logButton);
    mainLayout->addWidget(aiButton);
    mainLayout->addStretch(1);
    mainLayout->addWidget(terminalButton);
}

void ActivityBarWidget::initStyle()
{
    setStyleSheet(ThemeManager::activityBar());
}

void ActivityBarWidget::initConnections()
{
    connect(directoryButton, &QPushButton::clicked, this, [this]() { emit directoryPanelRequested(); });
    connect(fileButton, &QPushButton::clicked, this, [this]() { emit filePanelRequested(); });
    connect(logButton, &QPushButton::clicked, this, [this]() { emit logPanelRequested(); });
    connect(aiButton, &QPushButton::clicked, this, [this]() { emit aiPanelRequested(); });
    connect(terminalButton, &QPushButton::clicked, this, [this]() { emit terminalPanelRequested(); });
}

void ActivityBarWidget::setDirectoryChecked(bool checked)
{
    directoryButton->setChecked(checked);
}

void ActivityBarWidget::setFileChecked(bool checked)
{
    fileButton->setChecked(checked);
}

void ActivityBarWidget::setLogChecked(bool checked)
{
    logButton->setChecked(checked);
}

void ActivityBarWidget::setAiChecked(bool checked)
{
    aiButton->setChecked(checked);
}

void ActivityBarWidget::setTerminalChecked(bool checked)
{
    terminalButton->setChecked(checked);
}

void ActivityBarWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::activityBar());
}

