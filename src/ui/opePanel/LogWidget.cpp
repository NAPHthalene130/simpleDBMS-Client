#include "LogWidget.h"

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
    setStyleSheet(
        "LogWidget {"
        "    background-color: #111315;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 14px;"
        "}"
        "QLabel#logTitleLabel {"
        "    color: #D5D8DD;"
        "    font-weight: 700;"
        "    background-color: #2F3134;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 8px;"
        "    padding: 8px 10px;"
        "}"
        "QListWidget#logListWidget {"
        "    background-color: #111315;"
        "    color: #F0F0F0;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 10px;"
        "    padding: 6px;"
        "    outline: none;"
        "}"
        "QListWidget#logListWidget::item {"
        "    padding: 7px 8px;"
        "    margin: 1px 2px;"
        "    border-radius: 4px;"
        "}"
        "QListWidget#logListWidget::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QListWidget#logListWidget::item:selected {"
        "    background-color: #2F64A8;"
        "    color: #FFFFFF;"
        "}"
    );
}

void LogWidget::appendLog(const QString& message)
{
    if (message.isEmpty()) {
        return;
    }

    logList->addItem(message);
    logList->scrollToBottom();
}

