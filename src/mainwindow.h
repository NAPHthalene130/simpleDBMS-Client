#pragma once

#include <QMainWindow>

class AuthWidget;
class QStackedWidget;
class QWidget;

/**
 * @class MainWindow
 * @brief 应用程序主窗口类
 * @details 负责管理各个页面的切换以及全局窗口状态（如大小、标题等）
 * @author NAPH130
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param parent 父部件指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     * @author NAPH130
     */
    ~MainWindow();

    /**
     * @brief 切换当前显示的页面
     * @author NAPH130
     * @param widget 待显示的页面指针
     */
    void switchWidget(QWidget *widget);

private:
    QStackedWidget *stackedWidget; // 堆叠布局，用于页面切换
    AuthWidget *authWidget;        // 身份验证页面
};
