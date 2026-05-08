#include "TerminalWidget.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

TerminalWidget::TerminalWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      outputTextEdit(new QPlainTextEdit(this)),
      inputLineEdit(new QLineEdit(this)),
      executeButton(new QPushButton(tr("Execute"), this))
{
    auto *layout = new QVBoxLayout(this);
    auto *inputLayout = new QHBoxLayout();
    // 作者：YuzhSong
    // 仅优化终端面板的边距与间距，不影响 SQL 提交与回显链路。
    // 作者：YuzhSong
    // 终端面板内部采用统一 8px 边距与 4px 间距，使输入区与输出区边缘更规整，和主布局节奏一致。
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    inputLayout->setSpacing(0);

    outputTextEdit->setReadOnly(true);
    outputTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    outputTextEdit->setObjectName("terminalOutputTextEdit");
    inputLineEdit->setObjectName("terminalInputLineEdit");
    executeButton->setObjectName("terminalExecuteButton");
    inputLineEdit->setPlaceholderText(tr("Input SQL and press Enter or Execute..."));

    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->addWidget(inputLineEdit, 1);
    inputLayout->addWidget(executeButton, 0);

    layout->addWidget(outputTextEdit, 1);
    layout->addLayout(inputLayout, 0);

    connect(inputLineEdit, &QLineEdit::returnPressed, this, &TerminalWidget::submitSql);
    connect(executeButton, &QPushButton::clicked, this, &TerminalWidget::submitSql);

    applyTerminalStyle();
    appendMessage(tr("Terminal ready."));
}

MainWindow *TerminalWidget::getMainWindow() const
{
    return mainWindow;
}

void TerminalWidget::appendCommand(const QString &sql)
{
    outputTextEdit->appendPlainText(QString("[%1] > %2")
                                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                        .arg(sql));
}

void TerminalWidget::appendMessage(const QString &message)
{
    outputTextEdit->appendPlainText(QString("[%1] [INFO] %2")
                                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                        .arg(message));
}

void TerminalWidget::appendError(const QString &error)
{
    outputTextEdit->appendPlainText(QString("[%1] [ERROR] %2")
                                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                        .arg(error));
}

void TerminalWidget::clearTerminal()
{
    outputTextEdit->clear();
}

void TerminalWidget::submitSql()
{
    const QString sql = inputLineEdit->text().trimmed();
    if (sql.isEmpty()) {
        return;
    }

    appendCommand(sql);
    emit sqlSubmitted(sql);
    inputLineEdit->clear();
}

void TerminalWidget::applyTerminalStyle()
{
    // 作者：YuzhSong
    // 统一终端样式为深灰主题，并增强输入区、按钮和边框层次，不改动 submit/append 逻辑。
    // 作者：YuzhSong
    // TerminalWidget 外层即主面板，内部输出与输入仅保留弱边框，降低套娃感并统一 IDE 深色层级。
    setStyleSheet(
        "TerminalWidget {"
        "  background-color: #111315;"
        "  color: #F0F0F0;"
        "  border: 1px solid #1B1D20;"
        "  border-radius: 14px;"
        "}"
        "QPlainTextEdit#terminalOutputTextEdit {"
        "  background-color: #111315;"
        "  color: #F0F0F0;"
        "  border: 1px solid #1B1D20;"
        "  border-radius: 10px;"
        "  selection-background-color: #3D6FB6;"
        "}"
        "QLineEdit#terminalInputLineEdit {"
        "  background-color: #111315;"
        "  color: #F0F0F0;"
        "  border: 1px solid #1B1D20;"
        "  border-radius: 10px;"
        "  padding: 8px 10px;"
        "}"
        "QPushButton#terminalExecuteButton {"
        "  background-color: #111315;"
        "  color: #ffffff;"
        "  border: 1px solid #1B1D20;"
        "  border-radius: 10px;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#terminalExecuteButton:hover {"
        "  background-color: #1A1C1E;"
        "}"
    );
}
