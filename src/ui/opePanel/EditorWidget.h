#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class EditorWidget
 * @brief 代码编辑器组件类
 * @details 用于编写和执行 SQL 语句等
 * @author NAPH130
 */
class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH130
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit EditorWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

private:
    MainWindow *mainWindow; // 主窗口引用
};
