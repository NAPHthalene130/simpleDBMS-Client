#include "LogWidget.h"
#include "ui/ThemeManager.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

LogWidget::LogWidget(QWidget* parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , logList(nullptr)
{
    initUI();
    initStyle();
    appendLog(tr("Log 面板已初始化"));
}

void LogWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    titleLabel = new QLabel(tr("操作日志"), this);
    titleLabel->setObjectName("logTitleLabel");

    logList = new QListWidget(this);
    logList->setObjectName("logListWidget");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(logList, 1);
}

void LogWidget::initStyle()
{
    setStyleSheet(ThemeManager::sidePanel());
}

void LogWidget::appendLog(const QString& message)
{
    if (message.isEmpty()) {
        return;
    }

    logList->addItem(message);
    logList->scrollToBottom();
}

void LogWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::sidePanel());
}

