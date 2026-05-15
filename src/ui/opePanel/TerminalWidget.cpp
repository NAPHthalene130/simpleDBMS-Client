#include "TerminalWidget.h"
#include "ui/ThemeManager.h"

#include <QDateTime>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString buildTerminalEntry(const QString &level, const QString &payload)
{
    const QString prefix = QString("[%1] [%2]")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                               .arg(level);
    if (payload.contains('\n')) {
        return prefix + "\n" + payload;
    }
    return prefix + " " + payload;
}
} // namespace

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
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setStyleHint(QFont::Monospace);
    outputTextEdit->setFont(fixedFont);
    inputLineEdit->setFont(fixedFont);

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
    outputTextEdit->appendPlainText(buildTerminalEntry("INFO", message));
}

void TerminalWidget::appendError(const QString &error)
{
    outputTextEdit->appendPlainText(buildTerminalEntry("ERROR", error));
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
    setStyleSheet(ThemeManager::terminal());
}

void TerminalWidget::refreshTheme()
{
    applyTerminalStyle();

    const bool dark = ThemeManager::isDark();
    outputTextEdit->viewport()->setAutoFillBackground(true);
    QPalette vpPal = outputTextEdit->viewport()->palette();
    vpPal.setColor(QPalette::Base, dark ? QColor("#111315") : QColor("#F8F9FB"));
    vpPal.setColor(QPalette::Text, dark ? QColor("#F0F0F0") : QColor("#1C1E21"));
    outputTextEdit->viewport()->setPalette(vpPal);
}
