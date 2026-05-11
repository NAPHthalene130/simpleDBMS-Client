#pragma once

#include <QWidget>

class MainWindow;
class QLabel;
class QPushButton;
class QTableWidget;
class QString;

/**
 * @class TableWidget
 * @brief 数据表展示组件类
 * @details 用于展示数据库查询结果或表结构
 * @author Qi
 */
class TableWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author Qi
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit TableWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author Qi
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

    /**
     * @brief 设置查询结果
     * @author Qi
     * @param columns 查询结果列名列表
     * @param rows 查询结果二维数据
     */
    void setQueryResult(const std::vector<std::string> &columns,
                        const std::vector<std::vector<std::string>> &rows);

    /**
     * @brief 清空当前表格
     * @author Qi
     */
    void clearTable();

    /**
     * @brief 显示提示消息
     * @author Qi
     * @param message 需要显示的提示消息
     */
    void showMessage(const QString &message);

    void refreshTheme();

signals:
    /**
     * @brief 请求返回编辑器视图
     * @author Qi
     */
    void backToEditorRequested();

private:
    MainWindow *mainWindow; // 主窗口引用
    QLabel *messageLabel; // 消息显示标签
    QPushButton *backButton; // 返回编辑器按钮
    QTableWidget *tableViewWidget; // 表格视图组件
    std::vector<std::string> columns; // 查询结果列名
    std::vector<std::vector<std::string>> rows; // 查询结果数据

    void rebuildTableView();
    void applyTableStyle();
};
