#include "OpePanelWidget.h"

#include "opePanel/DirectoryWidget.h"
#include "opePanel/EditorWidget.h"
#include "opePanel/TableWidget.h"
#include "opePanel/TerminalWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
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
    auto *mainLayout = new QVBoxLayout(this);
    auto *headerFrame = new QFrame(this);
    auto *headerLayout = new QHBoxLayout(headerFrame);
    auto *titleLayout = new QVBoxLayout();
    auto *titleLabel = new QLabel(tr("工作区"), headerFrame);
    auto *subtitleLabel = new QLabel(tr("将文件、SQL 编辑与结果展示整合在同一条清晰流程中。"), headerFrame);
    auto *switchButtonLayout = new QHBoxLayout();
    auto *editorButton = new QPushButton(tr("编辑器"), headerFrame);
    auto *tableButton = new QPushButton(tr("数据表"), headerFrame);
    auto *contentLayout = new QHBoxLayout();
    auto *rightLayout = new QVBoxLayout();

    setObjectName("opePanelWidget");
    headerFrame->setObjectName("opePanelHeaderFrame");
    titleLabel->setObjectName("opePanelTitleLabel");
    subtitleLabel->setObjectName("opePanelSubtitleLabel");
    editorButton->setObjectName("opePanelTabButton");
    tableButton->setObjectName("opePanelTabButton");

    subtitleLabel->setWordWrap(true);

    mainDisplayStackedWidget->addWidget(editorWidget);
    mainDisplayStackedWidget->addWidget(tableWidget);
    mainDisplayStackedWidget->setCurrentWidget(editorWidget);

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);

    switchButtonLayout->addWidget(editorButton);
    switchButtonLayout->addWidget(tableButton);

    headerLayout->addLayout(titleLayout, 1);
    headerLayout->addLayout(switchButtonLayout);

    rightLayout->addWidget(mainDisplayStackedWidget, 1);
    rightLayout->addWidget(terminalWidget, 0);
    rightLayout->setSpacing(16);

    contentLayout->addWidget(directoryWidget, 0);
    contentLayout->addLayout(rightLayout, 1);
    contentLayout->setSpacing(16);

    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);
    mainLayout->addWidget(headerFrame, 0);
    mainLayout->addLayout(contentLayout, 1);

    setStyleSheet(
        "#opePanelWidget {"
        "    background-color: #f3f6fb;"
        "}"
        "#opePanelHeaderFrame {"
        "    background-color: white;"
        "    border: 1px solid #e2e8f3;"
        "    border-radius: 18px;"
        "}"
        "#opePanelTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 24px;"
        "    font-weight: 700;"
        "}"
        "#opePanelSubtitleLabel {"
        "    color: #667085;"
        "    font-size: 13px;"
        "}"
        "#opePanelTabButton {"
        "    min-width: 88px;"
        "    min-height: 38px;"
        "    padding: 0 14px;"
        "    border-radius: 12px;"
        "    border: 1px solid #d8e0ef;"
        "    background-color: #fbfcff;"
        "    color: #344054;"
        "    font-weight: 600;"
        "}"
        "#opePanelTabButton:hover {"
        "    background-color: #eef3ff;"
        "}"
    );

    connect(editorButton, &QPushButton::clicked, this, [this]() {
        switchWidget(editorWidget);
    });

    connect(tableButton, &QPushButton::clicked, this, [this]() {
        switchWidget(tableWidget);
    });
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
