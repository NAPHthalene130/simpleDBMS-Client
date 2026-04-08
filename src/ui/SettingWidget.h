#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class SettingWidget
 * @brief 设置页面类
 * @details 用于客户端相关的偏好设置
 * @author NAPH130
 */
class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit SettingWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH130
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    MainWindow *mainWindow; // 主窗口引用
};
