#include "EditorWidget.h"

#include <QLabel>
#include <QVBoxLayout>

EditorWidget::EditorWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("EditorWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}
