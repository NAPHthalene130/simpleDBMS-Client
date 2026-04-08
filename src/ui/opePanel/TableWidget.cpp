#include "TableWidget.h"

#include <QLabel>
#include <QVBoxLayout>

TableWidget::TableWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("TableWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}

MainWindow *TableWidget::getMainWindow() const
{
    return mainWindow;
}
