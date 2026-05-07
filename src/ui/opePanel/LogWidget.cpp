/**
 * @file LogWidget.cpp
 * @brief 左侧操作日志面板组件实现文件
 * @details 提供日志展示占位与 appendLog 接口，本阶段不接入真实日志业务。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#include "LogWidget.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @param parent 父组件指针
 * @author YuzhSong
 */
LogWidget::LogWidget(QWidget* parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , logList(nullptr)
{
    initUI();
    initStyle();
    appendLog(tr("Log 面板已初始化"));
}

/**
 * @brief 初始化界面
 * @details 按“标题 + 列表”结构构建基础日志区域。
 * @author YuzhSong
 */
void LogWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    titleLabel = new QLabel(tr("操作日志"), this);
    logList = new QListWidget(this);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(logList, 1);
}

/**
 * @brief 初始化样式
 * @details 与现有深灰主题保持一致，避免视觉风格跳变。
 * @author YuzhSong
 */
void LogWidget::initStyle()
{
    setStyleSheet(
        "LogWidget {"
        "    background-color: #252A31;"
        "    border-radius: 8px;"
        "}"
        "QLabel {"
        "    color: #D8D8D8;"
        "    font-weight: 600;"
        "}"
        "QListWidget {"
        "    background-color: #20242A;"
        "    color: #D0D5DB;"
        "    border: 1px solid #3A3F45;"
        "    border-radius: 6px;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 6px 8px;"
        "}"
    );
}

/**
 * @brief 追加日志
 * @details 将日志添加到末尾并滚动到最新项，便于用户查看最新操作信息。
 * @param message 日志文本
 * @author YuzhSong
 */
void LogWidget::appendLog(const QString& message)
{
    if (message.isEmpty()) {
        return;
    }

    logList->addItem(message);
    logList->scrollToBottom();
}

