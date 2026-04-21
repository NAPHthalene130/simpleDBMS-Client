#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();
};

#endif // SETTINGWIDGET_H