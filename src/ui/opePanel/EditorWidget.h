/**
 * @file EditorWidget.h
 * @brief SQL 编辑器主组件头文件
 * @details 该组件仅负责编辑区与文件编辑业务：打开、保存、另存为、运行 SQL、维护当前文件路径。
 *          左侧导航（已打开文件列表）由 DirectoryWidget 负责，二者通过信号槽协作。
 * @author YuzhSong
 * @date 2026-04-26
 */

#pragma once

#include <QToolBar>
#include <QToolButton>
#include <QWidget>

class MainWindow;
class QPushButton;
class SqlEditor;

/**
 * @class EditorWidget
 * @brief SQL 编辑器组件
 * @details 该类只关注“当前编辑文件”的编辑与执行流程，不包含左侧文件导航 UI。
 *          通过 fileOpened/currentFileChanged 通知外部更新 DirectoryWidget。
 * @author YuzhSong
 */
class EditorWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param mainWindow 主窗口指针
     * @param parent 父组件指针
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
     * @brief 执行 SQL 语句入口
     * @details 当前仍只执行当前编辑区 SQL，不处理其他已打开文件。
     * @author YuzhSong
     * @param sql 要执行的 SQL 文本
     */
    void executeSql(const QString& sql);

public slots:
    /**
     * @brief 切换到指定文件
     * @details 读取文件内容并显示到当前 SqlEditor，更新 currentFilePath 并发出 currentFileChanged。
     * @author YuzhSong
     * @param filePath 目标文件完整路径
     */
    void switchToFile(const QString& filePath);

signals:
    /**
     * @brief 文件打开成功信号
     * @details 用于通知 DirectoryWidget 将文件加入“已打开文件”列表。
     * @author YuzhSong
     * @param filePath 打开成功的文件完整路径
     */
    void fileOpened(const QString& filePath);

    /**
     * @brief 当前文件变化信号
     * @details 用于通知 DirectoryWidget 更新选中项。
     * @author YuzhSong
     * @param filePath 当前编辑文件完整路径
     */
    void currentFileChanged(const QString& filePath);

    /**
     * @brief 请求切换左侧目录面板显示状态
     * @details EditorWidget 不直接控制 DirectoryWidget 或外层 splitter，仅发起请求保持解耦。
     * @author YuzhSong
     */
    void toggleDirectoryRequested();

private slots:
    /**
     * @brief 新建文件槽函数
     * @author YuzhSong
     */
    void onNewFile();

    /**
     * @brief 打开文件槽函数
     * @author YuzhSong
     */
    void onOpenFile();

    /**
     * @brief 保存文件槽函数
     * @author YuzhSong
     */
    void onSaveFile();

    /**
     * @brief 另存为槽函数
     * @author YuzhSong
     */
    void onSaveFileAs();

    /**
     * @brief 运行 SQL 槽函数
     * @author YuzhSong
     */
    void onRunSql();

private:
    /**
     * @brief 初始化整体 UI
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化信号槽
     * @author YuzhSong
     */
    void initConnections();

    /**
     * @brief 设置编辑器基础显示属性
     * @author YuzhSong
     */
    void setupEditor();

    /**
     * @brief 设置工具栏
     * @author YuzhSong
     */
    void setupToolbar();

    /**
     * @brief 打开文件流程封装
     * @author YuzhSong
     * @return 成功返回 true，失败返回 false
     */
    bool openFile();

    /**
     * @brief 保存文件流程封装
     * @author YuzhSong
     * @return 成功返回 true，失败返回 false
     */
    bool saveFile();

    /**
     * @brief 另存为流程封装
     * @author YuzhSong
     * @return 成功返回 true，失败返回 false
     */
    bool saveFileAs();

    /**
     * @brief 运行当前编辑区 SQL 流程封装
     * @author YuzhSong
     */
    void runCurrentSql();

    /**
     * @brief 从文件读取内容到编辑器
     * @author YuzhSong
     * @param filePath 文件完整路径
     * @return 成功返回 true，失败返回 false
     */
    bool loadFile(const QString& filePath);

    /**
     * @brief 保存编辑器内容到文件
     * @author YuzhSong
     * @param filePath 文件完整路径
     * @return 成功返回 true，失败返回 false
     */
    bool saveFileToPath(const QString& filePath);

    /**
     * @brief 更新窗口标题显示当前文件名
     * @author YuzhSong
     * @param fileName 当前文件名
     */
    void updateWindowTitle(const QString& fileName);

    /**
     * @brief 规范化路径
     * @author YuzhSong
     * @param filePath 原始路径
     * @return 规范化后的绝对路径
     */
    QString normalizeFilePath(const QString& filePath) const;

private:
    MainWindow* mainWindow;            ///< 主窗口引用
    SqlEditor* sqlEditor;              ///< SQL 编辑器
    QToolBar* toolbar;                 ///< 顶部工具栏
    QToolButton* btnNew;               ///< 新建按钮
    QToolButton* btnOpen;              ///< 打开按钮
    QToolButton* btnSave;              ///< 保存按钮
    QToolButton* btnRun;               ///< 运行按钮
    QPushButton* btnToggleDirectory;   ///< 侧边栏开关按钮（仅发信号，不直接控制布局）
    QString currentFilePath;           ///< 当前编辑文件完整路径
};

