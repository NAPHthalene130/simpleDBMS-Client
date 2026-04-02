#include "AuthWidget.h"

#include <QLabel>
#include <QVBoxLayout>

AuthWidget::AuthWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("AuthWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}
