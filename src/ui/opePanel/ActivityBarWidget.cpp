/**
 * @file ActivityBarWidget.cpp
 * @author YuzhSong
 * @brief IDE 左侧 ActivityBar 实现文件
 * @details 仅负责按钮显示与请求信号，不处理具体面板布局逻辑
 * @module ui/opePanel
 */

#include "ActivityBarWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param parent 父组件指针
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
 * @details ActivityBar 固定窄宽度，四个按钮自上而下排布
 */
void ActivityBarWidget::initUI()
{
    setFixedWidth(48);

    auto* mainLayout = new QVBoxLayout(this);
    // 作者：YuzhSong
    // 调整 ActivityBar 留白，增强与侧边面板和编辑区的视觉分层。
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    fileButton = new QPushButton(tr("F"), this);
    logButton = new QPushButton(tr("L"), this);
    aiButton = new QPushButton(tr("A"), this);
    terminalButton = new QPushButton(tr("T"), this);

    fileButton->setToolTip(tr("File"));
    logButton->setToolTip(tr("Log"));
    aiButton->setToolTip(tr("AI"));
    terminalButton->setToolTip(tr("Terminal"));

    // 作者：YuzhSong
    // 统一使用 checkable 按钮，让外部协调层可以直接映射“面板是否打开”的状态。
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
 * @details 保持深灰主题和轻微高亮，不破坏既有整体视觉风格
 */
void ActivityBarWidget::initStyle()
{
    // 作者：YuzhSong
    // 统一左侧活动栏为深灰底色，hover 轻微提亮，checked 使用低饱和蓝色高亮。
    setStyleSheet(
        "ActivityBarWidget {"
        "    background-color: #202124;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 8px;"
        "}"
        "QPushButton {"
        "    min-height: 30px;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 6px;"
        "    background-color: #2B2D30;"
        "    color: #F0F0F0;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #34373C;"
        "}"
        "QPushButton:checked {"
        "    background-color: #3D6FB6;"
        "    border: 1px solid #4A83D6;"
        "    color: #FFFFFF;"
        "}"
    );
}

/**
 * @brief 初始化信号槽连接
 * @details 仅发出“请求”信号，真正显示/隐藏行为由 OpePanelWidget 协调
 */
void ActivityBarWidget::initConnections()
{
    connect(fileButton, &QPushButton::clicked, this, [this]() { emit filePanelRequested(); });
    connect(logButton, &QPushButton::clicked, this, [this]() { emit logPanelRequested(); });
    connect(aiButton, &QPushButton::clicked, this, [this]() { emit aiPanelRequested(); });
    connect(terminalButton, &QPushButton::clicked, this, [this]() { emit terminalPanelRequested(); });
}

/**
 * @brief 同步 File 按钮选中状态
 * @param checked true 表示 File 面板打开
 */
void ActivityBarWidget::setFileChecked(bool checked)
{
    fileButton->setChecked(checked);
}

/**
 * @brief 同步 Log 按钮选中状态
 * @param checked true 表示 Log 面板打开
 */
void ActivityBarWidget::setLogChecked(bool checked)
{
    logButton->setChecked(checked);
}

/**
 * @brief 同步 AI 按钮选中状态
 * @param checked true 表示 AI 面板打开
 */
void ActivityBarWidget::setAiChecked(bool checked)
{
    aiButton->setChecked(checked);
}

/**
 * @brief 同步 Terminal 按钮选中状态
 * @param checked true 表示 Terminal 面板打开
 */
void ActivityBarWidget::setTerminalChecked(bool checked)
{
    terminalButton->setChecked(checked);
}

/**
 * @brief 批量同步四个按钮状态
 * @param fileActive File 按钮状态
 * @param logActive Log 按钮状态
 * @param aiActive AI 按钮状态
 * @param terminalActive Terminal 按钮状态
 */
void ActivityBarWidget::updateSelectionState(bool fileActive, bool logActive, bool aiActive, bool terminalActive)
{
    setFileChecked(fileActive);
    setLogChecked(logActive);
    setAiChecked(aiActive);
    setTerminalChecked(terminalActive);
}
