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
    titleLabel->setStyleSheet("color: #CCCCCC; font-size: 16px;");

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addStretch();

    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1E1E;"
        "    color: #D4D4D4;"
        "}"
    ));
}

MainWindow *SettingWidget::getMainWindow() const
{
    return mainWindow;
}
