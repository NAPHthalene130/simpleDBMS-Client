#include "DebugWidgetSwitchWidget.h"

#include "mainwindow.h"
#include "ui/AuthWidget.h"
#include "ui/OpePanelWidget.h"
#include "ui/SettingWidget.h"
#include "ui/opePanel/EditorWidget.h"
#include "ui/opePanel/TableWidget.h"
#include "ui/opePanel/DirectoryWidget.h"
#include "ui/opePanel/TerminalWidget.h"

#include <QGridLayout>
#include <QPushButton>

DebugWidgetSwitchWidget::DebugWidgetSwitchWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QGridLayout(this);
    auto *authButton = new QPushButton(tr("Show AuthWidget"), this);
    auto *opePanelButton = new QPushButton(tr("Show OpePanelWidget"), this);
    auto *settingButton = new QPushButton(tr("Show SettingWidget"), this);
    auto *editorButton = new QPushButton(tr("Show EditorWidget"), this);
    auto *tableButton = new QPushButton(tr("Show TableWidget"), this);
    auto *directoryButton = new QPushButton(tr("Focus DirectoryWidget"), this);
    auto *terminalButton = new QPushButton(tr("Focus TerminalWidget"), this);

    layout->addWidget(authButton, 0, 0);
    layout->addWidget(opePanelButton, 0, 1);
    layout->addWidget(settingButton, 0, 2);
    layout->addWidget(editorButton, 1, 0);
    layout->addWidget(tableButton, 1, 1);
    layout->addWidget(directoryButton, 1, 2);
    layout->addWidget(terminalButton, 2, 0, 1, 3);

    connect(authButton, &QPushButton::clicked, this, [this]() {
        this->mainWindow->switchWidget(this->mainWindow->getAuthWidget());
    });

    connect(opePanelButton, &QPushButton::clicked, this, [this]() {
        this->mainWindow->switchWidget(this->mainWindow->getOpePanelWidget());
    });

    connect(settingButton, &QPushButton::clicked, this, [this]() {
        this->mainWindow->switchWidget(this->mainWindow->getSettingWidget());
    });

    connect(editorButton, &QPushButton::clicked, this, [this]() {
        auto *opePanelWidget = this->mainWindow->getOpePanelWidget();
        opePanelWidget->switchWidget(opePanelWidget->getEditorWidget());
        this->mainWindow->switchWidget(opePanelWidget);
    });

    connect(tableButton, &QPushButton::clicked, this, [this]() {
        auto *opePanelWidget = this->mainWindow->getOpePanelWidget();
        opePanelWidget->switchWidget(opePanelWidget->getTableWidget());
        this->mainWindow->switchWidget(opePanelWidget);
    });

    connect(directoryButton, &QPushButton::clicked, this, [this]() {
        auto *opePanelWidget = this->mainWindow->getOpePanelWidget();
        this->mainWindow->switchWidget(opePanelWidget);
        opePanelWidget->getDirectoryWidget()->setFocus();
    });

    connect(terminalButton, &QPushButton::clicked, this, [this]() {
        auto *opePanelWidget = this->mainWindow->getOpePanelWidget();
        this->mainWindow->switchWidget(opePanelWidget);
        opePanelWidget->getTerminalWidget()->setFocus();
    });
}

MainWindow *DebugWidgetSwitchWidget::getMainWindow() const
{
    return mainWindow;
}
