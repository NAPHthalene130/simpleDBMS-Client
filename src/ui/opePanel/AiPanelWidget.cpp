/**
 * @file AiPanelWidget.cpp
 * @author YuzhSong
 * @brief 右侧 AI 面板占位组件实现文件
 * @details 当前仅提供占位内容展示，不引入真实 AI 交互逻辑
 * @module ui/opePanel
 */

#include "AiPanelWidget.h"

#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param parent 父组件指针
 */
AiPanelWidget::AiPanelWidget(QWidget* parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , placeholderLabel(nullptr)
    , hintLabel(nullptr)
    , mainLayout(nullptr)
{
    initUI();
    initStyle();
}

/**
 * @brief 初始化界面结构
 * @details
 * 1. 提供标题 + 主文案 + 说明文案。
 * 2. 该组件默认由外部控制隐藏，本类只负责内容组织。
 */
void AiPanelWidget::initUI()
{
    mainLayout = new QVBoxLayout(this);
    // 作者：YuzhSong
    // 控制 AI 面板内边距与间距，保证右侧面板与主编辑区视觉一致。
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    titleLabel = new QLabel(tr("AI Assistant"), this);
    placeholderLabel = new QLabel(tr("暂未实现"), this);
    hintLabel = new QLabel(tr("后续可用于 SQL 解释、SQL 优化、报错解释等功能。"), this);

    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    placeholderLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hintLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    hintLabel->setWordWrap(true);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(placeholderLabel);
    mainLayout->addWidget(hintLabel);
    mainLayout->addStretch(1);
}

/**
 * @brief 初始化样式
 * @details 采用深灰背景、轻微圆角，保持与工作区整体风格一致
 */
void AiPanelWidget::initStyle()
{
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
    // 作者：YuzhSong
    // AI 面板采用深灰背景 + 次级说明文本 + 轻微赛博感高亮，不引入业务逻辑。
    setStyleSheet(
        "AiPanelWidget {"
        "    background-color: #252629;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 8px;"
        "}"
        "QLabel {"
        "    color: #A8A8A8;"
        "}"
        "QLabel:first-child {"
        "    color: #E8ECF5;"
        "    font-size: 15px;"
        "    font-weight: 700;"
        "}"
        "QLabel:nth-child(2) {"
        "    color: #7FB2FF;"
        "    font-weight: 600;"
        "}"
    );
}
