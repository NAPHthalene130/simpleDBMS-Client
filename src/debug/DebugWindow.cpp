#include "DebugWindow.h"

#include "DebugLogWidget.h"
#include "DebugWidgetSwitchWidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

DebugWindow::DebugWindow(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      debugWidgetSwitchWidget(new DebugWidgetSwitchWidget(mainWindow, this)),
      debugLogWidget(new DebugLogWidget(mainWindow, this)),
      stackedWidget(new QStackedWidget(this))
{
    setWindowTitle(tr("DebugWindow"));
    resize(900, 600);

    auto *mainLayout = new QVBoxLayout(this);
    auto *topButtonLayout = new QHBoxLayout();
    auto *widgetSwitchPageButton = new QPushButton(tr("WidgetSwitch"), this);
    auto *logPageButton = new QPushButton(tr("Log"), this);

    topButtonLayout->addWidget(widgetSwitchPageButton);
    topButtonLayout->addWidget(logPageButton);
    topButtonLayout->addStretch();

    stackedWidget->addWidget(debugWidgetSwitchWidget);
    stackedWidget->addWidget(debugLogWidget);

    mainLayout->addLayout(topButtonLayout);
    mainLayout->addWidget(stackedWidget, 1);

    switchWidget(debugWidgetSwitchWidget);

    connect(widgetSwitchPageButton, &QPushButton::clicked, this, [this]() {
        switchWidget(debugWidgetSwitchWidget);
    });

    connect(logPageButton, &QPushButton::clicked, this, [this]() {
        switchWidget(debugLogWidget);
    });

    setStyleSheet(QString(
        "QWidget {"
        "    background-color: #1E1E1E;"
        "    color: #D4D4D4;"
        "}"
        "QPushButton {"
        "    background-color: #3C3C3C;"
        "    color: #E0E0E0;"
        "    border: 1px solid #555555;"
        "    border-radius: 4px;"
        "    padding: 6px 14px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #505050;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #404040;"
        "}"
    ));
}

void DebugWindow::switchWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return;
    }

    const int widgetIndex = stackedWidget->indexOf(widget);
    if (widgetIndex < 0) {
        return;
    }

    stackedWidget->setCurrentIndex(widgetIndex);
}

MainWindow *DebugWindow::getMainWindow() const
{
    return mainWindow;
}
