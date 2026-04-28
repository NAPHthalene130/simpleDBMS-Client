#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class DebugLogWidget
 * @brief 调试日志页面
 * @details 用于展示调试日志信息，当前仅提供页面骨架
 * @author NAPH1301
 */
class DebugLogWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口指针
     * @param parent 父部件指针
     */
    explicit DebugLogWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH1301
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    MainWindow *mainWindow;
};
