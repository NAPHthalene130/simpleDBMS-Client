#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class DebugWidgetSwitchWidget
 * @brief 调试窗口组件切换页面
 * @details 提供按钮用于切换主窗口中的各个页面及操作面板子页面
 * @author NAPH130
 */
class DebugWidgetSwitchWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针
     * @param parent 父部件指针
     */
    explicit DebugWidgetSwitchWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH130
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    MainWindow *mainWindow;
};
