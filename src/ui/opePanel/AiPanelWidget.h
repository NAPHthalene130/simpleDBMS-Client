#pragma once

#include <QWidget>

class QLabel;
class QPlainTextEdit;
class QVBoxLayout;

class AiPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AiPanelWidget(QWidget* parent = nullptr);

private:
    void initUI();
    void initStyle();

private:
    QLabel* titleLabel;
    QPlainTextEdit* contentTextEdit;
    QVBoxLayout* mainLayout;
};

