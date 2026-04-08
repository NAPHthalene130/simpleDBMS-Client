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
