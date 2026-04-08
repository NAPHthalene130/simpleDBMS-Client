#pragma once

#include <QWidget>

class MainWindow;
class QStackedWidget;
class DirectoryWidget;
class EditorWidget;
class TableWidget;
class TerminalWidget;

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

    /**
     * @brief 切换中央主显示区组件
     * @author NAPH130
     * @param widget 目标显示组件
     */
    void switchWidget(QWidget *widget);

    /**
     * @brief 获取主窗口指针
     * @author NAPH130
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

    /**
     * @brief 获取左侧目录组件
     * @author NAPH130
     * @return 目录组件指针
     */
    DirectoryWidget *getDirectoryWidget() const;

    /**
     * @brief 获取中央编辑器组件
     * @author NAPH130
     * @return 编辑器组件指针
     */
    EditorWidget *getEditorWidget() const;

    /**
     * @brief 获取中央数据表格组件
     * @author NAPH130
     * @return 数据表格组件指针
     */
    TableWidget *getTableWidget() const;

    /**
     * @brief 获取下侧终端组件
     * @author NAPH130
     * @return 终端组件指针
     */
    TerminalWidget *getTerminalWidget() const;

    /**
     * @brief 获取中央主显示区容器
     * @author NAPH130
     * @return 主显示区容器指针
     */
    QStackedWidget *getMainDisplayStackedWidget() const;

private:
    MainWindow *mainWindow; // 主窗口引用
    DirectoryWidget *directoryWidget; // 左侧目录组件
    EditorWidget *editorWidget; // 中央编辑器组件
    TableWidget *tableWidget; // 中央数据表格组件
    TerminalWidget *terminalWidget; // 下侧终端组件
    QStackedWidget *mainDisplayStackedWidget; // 中央主显示区容器
};
