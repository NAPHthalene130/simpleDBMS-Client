#include "TerminalWidget.h"

#include <QFrame>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

TerminalWidget::TerminalWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("活动控制台"), this);
    auto *consoleFrame = new QFrame(this);
    auto *consoleLayout = new QVBoxLayout(consoleFrame);
    auto *console = new QPlainTextEdit(consoleFrame);

    titleLabel->setObjectName("terminalTitleLabel");
    consoleFrame->setObjectName("terminalCard");

    console->setReadOnly(true);
    console->setFixedHeight(150);
    console->setPlainText(
        "[17:20:15][INFO] 客户端初始化完成。\n"
        "[17:20:16][DEBUG] 界面主题加载完成。\n"
        "[17:20:17][INFO] 正在等待服务器连接..."
    );

    consoleLayout->setContentsMargins(0, 0, 0, 0);
    consoleLayout->addWidget(console);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(titleLabel);
    layout->addWidget(consoleFrame);

    setStyleSheet(
        "#terminalTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 18px;"
        "    font-weight: 700;"
        "}"
        "#terminalCard {"
        "    background-color: #081120;"
        "    border: 1px solid #172033;"
        "    border-radius: 16px;"
        "}"
        "QPlainTextEdit {"
        "    border: none;"
        "    border-radius: 16px;"
        "    background-color: transparent;"
        "    color: #9ee6b1;"
        "    padding: 14px;"
        "}"
    );
}

MainWindow *TerminalWidget::getMainWindow() const
{
    return mainWindow;
}
