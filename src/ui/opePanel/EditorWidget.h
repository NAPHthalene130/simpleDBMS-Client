/**
 * @file EditorWidget.h
 * @brief SQL 编辑器组件头文件
 * @details 提供 SQL 编辑、文件操作（新建/打开/保存/另存为）和 SQL 执行功能。
 *          上方工具栏包含新建、打开、保存、Run 按钮。
 *          中间编辑区使用 SqlEditor，支持行号显示、语法高亮和自动缩进。
 * @author YuzhSong
 * @date 2026-04-09
 */

#pragma once

#include <QToolBar>
#include <QToolButton>
#include <QWidget>

class MainWindow;
class SqlEditor;

/**
 * @class EditorWidget
 * @brief SQL 编辑器组件
 * @details 封装了 SQL 编辑区域，提供文件操作和 SQL 执行功能。
 *          - 上方工具栏：新建 / 打开 / 保存 / Run 按钮
 *          - 中间编辑区：SqlEditor（支持行号、语法高亮、自动缩进）
 *          - 支持快捷键：Ctrl+S 保存、Ctrl+O 打开、Ctrl+Enter 执行 SQL
 * @author YuzhSong
 */
class EditorWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit EditorWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     * @author YuzhSong
     */
    ~EditorWidget();

    /**
     * @brief 获取主窗口指针
     * @author YuzhSong
     * @return 主窗口指针
     */
    MainWindow* getMainWindow() const;

    /**
     * @brief 执行 SQL 语句接口
     * @details 当前仅做调试输出，后续用于连接 DBMS
     * @author YuzhSong
     * @param sql 要执行的 SQL 语句
     */
    void executeSql(const QString& sql);

private slots:
    /**
     * @brief 新建 SQL 文本槽函数
     * @details 清空编辑器内容，重置当前文件路径
     * @author YuzhSong
     */
    void onNewFile();

    /**
     * @brief 打开 .sql 文件槽函数
     * @details 弹出文件选择对话框，读取选中的 .sql 文件内容到编辑器
     * @author YuzhSong
     */
    void onOpenFile();

    /**
     * @brief 保存文件槽函数
     * @details 如果当前文件已有路径，直接保存；否则弹出另存为对话框
     * @author YuzhSong
     */
    void onSaveFile();

    /**
     * @brief 另存为文件槽函数
     * @details 弹出另存为对话框，保存当前编辑内容
     * @author YuzhSong
     */
    void onSaveFileAs();

    /**
     * @brief 运行 SQL 槽函数
     * @details 获取当前编辑器中的 SQL 文本，调用 executeSql 方法执行
     * @author YuzhSong
     */
    void onRunSql();

private:
    /**
     * @brief 初始化 UI 布局
     * @details 创建垂直布局，添加工具栏和编辑区
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     * @details 连接按钮点击事件和快捷键到对应的槽函数
     * @author YuzhSong
     */
    void initConnections();

    /**
     * @brief 设置编辑器样式和占位提示
     * @details 配置 SqlEditor 的字体、默认提示文本等
     * @author YuzhSong
     */
    void setupEditor();

    /**
     * @brief 设置工具栏和按钮
     * @details 创建工具栏并添加新建/打开/保存/运行按钮
     * @author YuzhSong
     */
    void setupToolbar();

    /**
     * @brief 从文件读取内容到编辑器
     * @author YuzhSong
     * @param filePath 文件路径
     * @return 是否读取成功
     */
    bool loadFile(const QString& filePath);

    /**
     * @brief 保存编辑器内容到文件
     * @author YuzhSong
     * @param filePath 文件路径
     * @return 是否保存成功
     */
    bool saveFile(const QString& filePath);

    /**
     * @brief 更新窗口标题显示当前文件名
     * @author YuzhSong
     * @param fileName 当前文件名
     */
    void updateWindowTitle(const QString& fileName);

private:
    MainWindow* mainWindow;    ///< 主窗口引用

    SqlEditor* sqlEditor;      ///< SQL 编辑区（支持行号、高亮、缩进）
    QToolBar* toolbar;         ///< 工具栏

    QToolButton* btnNew;       ///< 新建按钮
    QToolButton* btnOpen;      ///< 打开按钮
    QToolButton* btnSave;      ///< 保存按钮
    QToolButton* btnRun;       ///< 运行按钮

    QString currentFilePath;   ///< 当前文件路径
};
