#include "AiPanelWidget.h"
#include "ui/ThemeManager.h"

#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

AiPanelWidget::AiPanelWidget(QWidget* parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , contentTextEdit(nullptr)
    , mainLayout(nullptr)
{
    initUI();
    initStyle();
}

void AiPanelWidget::initUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 12, 0, 0);
    mainLayout->setSpacing(8);

    titleLabel = new QLabel(tr("AI Assistant"), this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    contentTextEdit = new QPlainTextEdit(this);
    contentTextEdit->setObjectName("aiAssistantContentTextEdit");
    contentTextEdit->setReadOnly(true);
    contentTextEdit->setPlainText(
        tr("暂未实现\n\n后续可用SQL 解释、SQL 优化、报错解释等功能")
    );

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(contentTextEdit, 1);
}

void AiPanelWidget::initStyle()
{
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);

    setStyleSheet(ThemeManager::sidePanel());
}

void AiPanelWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::sidePanel());
}
