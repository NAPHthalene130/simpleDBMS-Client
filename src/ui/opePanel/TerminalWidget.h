#pragma once

#include <QString>

#include <QWidget>

class MainWindow;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

/**
 * @class TerminalWidget
 * @brief 终端输出组件类
 * @details 用于展示执行日志、错误信息等终端输出
 * @author Qi
 */
class TerminalWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author Qi
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit TerminalWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author Qi
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

    /**
     * @brief 追加用户执行命令记录
     * @author Qi
     * @param sql SQL 文本
     */
    void appendCommand(const QString &sql);

    /**
     * @brief 追加普通消息
     * @author Qi
     * @param message 普通提示信息
     */
    void appendMessage(const QString &message);

    /**
     * @brief 追加错误消息
     * @author Qi
     * @param error 错误信息
     */
    void appendError(const QString &error);

    /**
     * @brief 清空终端输出
     * @author Qi
     */
    void clearTerminal();

    void refreshTheme();

signals:
    /**
     * @brief SQL 提交信号
     * @author Qi
     * @param sql 用户提交的 SQL 文本
     */
    void sqlSubmitted(const QString &sql);

private:
    MainWindow *mainWindow; // 主窗口引用
    QPlainTextEdit *outputTextEdit; // 终端输出区
    QLineEdit *inputLineEdit; // SQL 输入框
    QPushButton *executeButton; // 执行按钮

    void submitSql();
    void applyTerminalStyle();
};
