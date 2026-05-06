#include "DebugLogWidget.h"

#include <QLabel>
#include <QVBoxLayout>

DebugLogWidget::DebugLogWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("DebugLogWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #CCCCCC; font-size: 16px;");
    layout->addWidget(titleLabel);
}

MainWindow *DebugLogWidget::getMainWindow() const
{
    return mainWindow;
}
