#include "SettingWidget.h"

#include <QLabel>
#include <QVBoxLayout>

SettingWidget::SettingWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("SettingWidget"), this);

    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();
}

MainWindow *SettingWidget::getMainWindow() const
{
    return mainWindow;
}
