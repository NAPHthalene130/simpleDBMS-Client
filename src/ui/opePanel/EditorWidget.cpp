#include "EditorWidget.h"

#include <QFrame>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

EditorWidget::EditorWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("SQL 编辑器"), this);
    auto *hintLabel = new QLabel(tr("一个小巧思：预置示例查询，让用户一打开就知道这里可以直接开始写。"), this);
    auto *editorFrame = new QFrame(this);
    auto *editorLayout = new QVBoxLayout(editorFrame);
    auto *editor = new QPlainTextEdit(editorFrame);

    setObjectName("editorWidget");
    titleLabel->setObjectName("editorTitleLabel");
    hintLabel->setObjectName("editorHintLabel");
    editorFrame->setObjectName("editorCard");

    editor->setPlainText(
        "SELECT id, user_name, created_at\n"
        "FROM user_profile\n"
        "WHERE status = 'active'\n"
        "ORDER BY created_at DESC\n"
        "LIMIT 20;"
    );

    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->addWidget(editor);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(editorFrame, 1);

    setStyleSheet(
        "#editorTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 18px;"
        "    font-weight: 700;"
        "}"
        "#editorHintLabel {"
        "    color: #667085;"
        "    font-size: 12px;"
        "}"
        "#editorCard {"
        "    background-color: white;"
        "    border: 1px solid #e2e8f3;"
        "    border-radius: 16px;"
        "}"
        "QPlainTextEdit {"
        "    border: none;"
        "    border-radius: 16px;"
        "    background-color: #0f172a;"
        "    color: #dbeafe;"
        "    padding: 16px;"
        "    selection-background-color: #334155;"
        "}"
    );
}

MainWindow *EditorWidget::getMainWindow() const
{
    return mainWindow;
}
