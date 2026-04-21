#include "settingwidget.h"
#include <QLabel>
#include <QVBoxLayout>

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *placeholder = new QLabel("设置功能待开发 (预留接口)", this);
    placeholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholder);
}

SettingWidget::~SettingWidget()
{
}