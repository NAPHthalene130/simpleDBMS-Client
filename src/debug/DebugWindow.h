#pragma once

#include <QWidget>

class DebugLogWidget;
class DebugWidgetSwitchWidget;
class MainWindow;
class QStackedWidget;

/**
 * @class DebugWindow
 * @brief 调试窗口类
 * @details 上方为功能选择区域，下方为调试功能页面区域
 * @author NAPH130
 */
class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针
     * @param parent 父部件指针
     */
    explicit DebugWindow(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 切换调试功能页面
     * @author NAPH130
     * @param widget 目标调试页面
     */
    void switchWidget(QWidget *widget);

    /**
     * @brief 获取主窗口指针
     * @author NAPH130
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    MainWindow *mainWindow;
    DebugWidgetSwitchWidget *debugWidgetSwitchWidget;
    DebugLogWidget *debugLogWidget;
    QStackedWidget *stackedWidget;
};
