#pragma once

#include <QWidget>

class MainWindow;

/**
 * @class TableWidget
 * @brief 数据表展示组件类
 * @details 用于展示数据库查询结果或表结构
 * @author NAPH1301
 */
class TableWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit TableWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH1301
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

private:
    MainWindow *mainWindow; // 主窗口引用
};
