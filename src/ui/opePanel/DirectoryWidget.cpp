#include "DirectoryWidget.h"

#include <QLabel>
#include <QVBoxLayout>

DirectoryWidget::DirectoryWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("DirectoryWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}
