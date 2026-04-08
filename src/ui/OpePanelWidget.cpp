#include "OpePanelWidget.h"

#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QVBoxLayout>

OpePanelWidget::OpePanelWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      directoryWidget(new DirectoryWidget(mainWindow, this)),
      editorWidget(new EditorWidget(mainWindow, this)),
      tableWidget(new TableWidget(mainWindow, this)),
      terminalWidget(new TerminalWidget(mainWindow, this)),
      mainDisplayStackedWidget(new QStackedWidget(this))
{
    auto *mainLayout = new QHBoxLayout(this);
    auto *rightLayout = new QVBoxLayout();

    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    rightLayout->addWidget(mainDisplayStackedWidget, 1);
    rightLayout->addWidget(terminalWidget, 0);

    mainLayout->addWidget(directoryWidget, 0);
    mainLayout->addLayout(rightLayout, 1);
}

void OpePanelWidget::switchWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return;
    }

    const int widgetIndex = mainDisplayStackedWidget->indexOf(widget);
    if (widgetIndex < 0) {
        return;
    }

    mainDisplayStackedWidget->setCurrentIndex(widgetIndex);
}

MainWindow *OpePanelWidget::getMainWindow() const
{
    return mainWindow;
}

DirectoryWidget *OpePanelWidget::getDirectoryWidget() const
{
    return directoryWidget;
}

EditorWidget *OpePanelWidget::getEditorWidget() const
{
    return editorWidget;
}

TableWidget *OpePanelWidget::getTableWidget() const
{
    return tableWidget;
}

TerminalWidget *OpePanelWidget::getTerminalWidget() const
{
    return terminalWidget;
}

QStackedWidget *OpePanelWidget::getMainDisplayStackedWidget() const
{
    return mainDisplayStackedWidget;
}
