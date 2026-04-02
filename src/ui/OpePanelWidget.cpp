#include "OpePanelWidget.h"

#include <QLabel>
#include <QVBoxLayout>

OpePanelWidget::OpePanelWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("OpePanelWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}
