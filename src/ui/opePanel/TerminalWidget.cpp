#include "TerminalWidget.h"

#include <QLabel>
#include <QVBoxLayout>

TerminalWidget::TerminalWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("TerminalWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}
