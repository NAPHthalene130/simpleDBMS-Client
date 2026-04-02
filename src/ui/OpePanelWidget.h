#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class OpePanelWidget
 * @brief 操作面板页面类
 * @details 用户登录后的主操作界面，包含目录树、编辑器、终端、数据表等子面板
 * @author NAPH130
 */
class OpePanelWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit OpePanelWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

private:
    MainWindow *mainWindow; // 主窗口引用
};
