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

    outputTextEdit->setReadOnly(true);
    outputTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    inputLineEdit->setPlaceholderText(tr("Input SQL and press Enter or Execute..."));

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
    setStyleSheet(
        "QWidget { background-color: #1e1e1e; color: #d4d4d4; }"
        "QPlainTextEdit {"
        "  background-color: #111111;"
        "  color: #d4d4d4;"
        "  border: 1px solid #3c3c3c;"
        "  selection-background-color: #264f78;"
        "}"
        "QLineEdit {"
        "  background-color: #252526;"
        "  color: #d4d4d4;"
        "  border: 1px solid #3c3c3c;"
        "  padding: 4px;"
        "}"
        "QPushButton {"
        "  background-color: #0e639c;"
        "  color: #ffffff;"
        "  border: 1px solid #1177bb;"
        "  padding: 4px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #1177bb;"
        "}"
    );
}
