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
    mainLayout->setContentsMargins(14, 14, 14, 14);
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
    setStyleSheet(
        "AiPanelWidget {"
        "    background-color: #252A31;"
        "    border: 1px solid #383D44;"
        "    border-radius: 8px;"
        "}"
        "QLabel {"
        "    color: #D4D8DE;"
        "}"
        "QLabel:first-child {"
        "    color: #F0F3F8;"
        "    font-size: 15px;"
        "    font-weight: 700;"
        "}"
    );
}

