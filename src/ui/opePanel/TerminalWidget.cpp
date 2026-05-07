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
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);
    inputLayout->setSpacing(6);

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
    // 作者：YuzhSong
    // 统一终端样式为深灰主题，并增强输入区、按钮和边框层次，不改动 submit/append 逻辑。
    setStyleSheet(
        "QWidget { background-color: #202124; color: #F0F0F0; border-radius: 8px; }"
        "QPlainTextEdit {"
        "  background-color: #1E1F22;"
        "  color: #F0F0F0;"
        "  border: 1px solid #3A3D42;"
        "  border-radius: 6px;"
        "  selection-background-color: #3D6FB6;"
        "}"
        "QLineEdit {"
        "  background-color: #252629;"
        "  color: #F0F0F0;"
        "  border: 1px solid #3A3D42;"
        "  border-radius: 6px;"
        "  padding: 5px 8px;"
        "}"
        "QPushButton {"
        "  background-color: #2B2D30;"
        "  color: #ffffff;"
        "  border: 1px solid #3A3D42;"
        "  border-radius: 6px;"
        "  padding: 5px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #34373C;"
        "}"
    );
}
