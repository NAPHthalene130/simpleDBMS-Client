#include "mainwindow.h"

#include <QStackedWidget>

#include "ui/AuthWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      stackedWidget(new QStackedWidget(this)),
      authWidget(new AuthWidget(this))
{
    resize(1200, 800);
    setCentralWidget(stackedWidget);
    switchWidget(authWidget);
}

MainWindow::~MainWindow() = default;

void MainWindow::switchWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return;
    }

    if (stackedWidget->indexOf(widget) < 0) {
        stackedWidget->addWidget(widget);
    }

    stackedWidget->setCurrentWidget(widget);
}
