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
    mainLayout->setContentsMargins(10, 10, 10, 10);
    // 作者：YuzhSong
    // 调整日志区内部间距，提升列表可读性与现代 IDE 的紧凑感平衡。
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
    // 作者：YuzhSong
    // 日志面板统一深灰底色与边框，日志项增加轻微间距与选中反馈。
    // 作者：YuzhSong
    // 外层主面板与内部列表采用低对比边界，强调 IDE 层级而不是输入框套娃边框。
    // 作者：YuzhSong
    // Log 与 Directory 统一采用 14px 面板圆角，内部列表使用 8px 次级圆角，保证侧栏视觉一致性。
    setStyleSheet(
        "LogWidget {"
        "    background-color: #252629;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 14px;"
        "}"
        "QLabel {"
        "    color: #F0F0F0;"
        "    font-weight: 600;"
        "}"
        "QListWidget {"
        "    background-color: #1F2023;"
        "    color: #F0F0F0;"
        "    border: 1px solid #2F3136;"
        "    border-radius: 8px;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 7px 8px;"
        "    margin: 1px 2px;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #4C5A7A;"
        "    color: #FFFFFF;"
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
