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
    // 作者：YuzhSong
    // ActivityBar 固定为窄竖栏，严格贴合草图中的 56px 宽度。
    setFixedWidth(35);

    auto* mainLayout = new QVBoxLayout(this);
    // 作者：YuzhSong
    // 调整 ActivityBar 留白，增强与侧边面板和编辑区的视觉分层。
    mainLayout->setContentsMargins(2, 8, 2, 8);
    mainLayout->setSpacing(8);

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
    fileButton->setFixedSize(30, 30);
    logButton->setFixedSize(30, 30);
    aiButton->setFixedSize(30, 30);
    terminalButton->setFixedSize(30, 30);

    mainLayout->addWidget(fileButton);
    mainLayout->addWidget(logButton);
    mainLayout->addWidget(aiButton);
    // 作者：YuzhSong
    // Terminal 按钮固定靠下，符合 IDE 工具栏常见的上方主入口、下方终端入口分区。
    mainLayout->addStretch(1);
    mainLayout->addWidget(terminalButton);
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
        "    background-color: #111315;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 14px;"
        "}"
        "QPushButton {"
        "    min-width: 30px;"
        "    min-height: 30px;"
        "    border: 1px solid transparent;"
        "    border-radius: 7px;"
        "    background-color: #1A1C1E;"
        "    color: #C8CBD2;"
        "    font-weight: 600;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #26292D;"
        "    border-color: #2B3035;"
        "}"
        "QPushButton:checked {"
        "    background-color: #2F64A8;"
        "    border: 1px solid #2F64A8;"
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
