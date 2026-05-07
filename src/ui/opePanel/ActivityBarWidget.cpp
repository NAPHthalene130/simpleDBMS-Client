/**
 * @file ActivityBarWidget.cpp
 * @brief IDE 风格左侧 ActivityBar 组件实现文件
 * @details 提供窄按钮栏，仅负责按钮展示和请求信号发射，不承担面板业务逻辑。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#include "ActivityBarWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param parent 父组件指针
 * @author YuzhSong
 */
ActivityBarWidget::ActivityBarWidget(QWidget* parent)
    : QWidget(parent)
    , fileButton(nullptr)
    , logButton(nullptr)
    , aiButton(nullptr)
    , terminalButton(nullptr)
{
    initUI();
    initStyle();
    initConnections();
}

/**
 * @brief 初始化界面结构
 * @details ActivityBar 固定为窄栏，按钮按垂直方向排列。
 * @author YuzhSong
 */
void ActivityBarWidget::initUI()
{
    setFixedWidth(48);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 8, 6, 8);
    mainLayout->setSpacing(8);

    fileButton = new QPushButton(tr("F"), this);
    logButton = new QPushButton(tr("L"), this);
    aiButton = new QPushButton(tr("A"), this);
    terminalButton = new QPushButton(tr("T"), this);

    fileButton->setToolTip(tr("File"));
    logButton->setToolTip(tr("Log"));
    aiButton->setToolTip(tr("AI"));
    terminalButton->setToolTip(tr("Terminal"));

    fileButton->setCheckable(true);
    logButton->setCheckable(true);
    aiButton->setCheckable(true);
    terminalButton->setCheckable(true);

    mainLayout->addWidget(fileButton);
    mainLayout->addWidget(logButton);
    mainLayout->addWidget(aiButton);
    mainLayout->addWidget(terminalButton);
    mainLayout->addStretch(1);
}

/**
 * @brief 初始化样式
 * @details 采用深灰配色，使用 hover 与 checked 状态突出当前操作入口。
 * @author YuzhSong
 */
void ActivityBarWidget::initStyle()
{
    setStyleSheet(
        "ActivityBarWidget {"
        "    background-color: #1C1F24;"
        "}"
        "QPushButton {"
        "    min-height: 30px;"
        "    border: 1px solid #3A3F45;"
        "    border-radius: 6px;"
        "    background-color: #2A2F36;"
        "    color: #D0D5DB;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #353C45;"
        "}"
        "QPushButton:checked {"
        "    background-color: #2F3D63;"
        "    border: 1px solid #4F7BFF;"
        "    color: #FFFFFF;"
        "}"
    );
}

/**
 * @brief 初始化信号槽连接
 * @details 本组件只发出请求信号，真正的显示/隐藏逻辑由外部协调层处理。
 * @author YuzhSong
 */
void ActivityBarWidget::initConnections()
{
    connect(fileButton, &QPushButton::clicked, this, [this]() {
        emit filePanelRequested();
    });

    connect(logButton, &QPushButton::clicked, this, [this]() {
        emit logPanelRequested();
    });

    connect(aiButton, &QPushButton::clicked, this, [this]() {
        emit aiPanelRequested();
    });

    connect(terminalButton, &QPushButton::clicked, this, [this]() {
        emit terminalPanelRequested();
    });
}

/**
 * @brief 同步按钮选中态
 * @details
 * 1. 本阶段仅同步 File / Log 选中态。
 * 2. AI / Terminal 暂不接入真实面板逻辑，默认保持未选中。
 * @param fileActive File 按钮是否高亮
 * @param logActive Log 按钮是否高亮
 * @author YuzhSong
 */
void ActivityBarWidget::updateSelectionState(bool fileActive, bool logActive)
{
    fileButton->setChecked(fileActive);
    logButton->setChecked(logActive);

    // 预留按钮暂不接入真实逻辑，避免误导用户产生“已实现面板”的认知。
    aiButton->setChecked(false);
    terminalButton->setChecked(false);
}

