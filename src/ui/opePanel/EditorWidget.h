/**
 * @file EditorWidget.h
 * @brief SQL 编辑器主组件头文件
 * @details 该组件仅负责编辑区与文件编辑业务：打开、保存、另存为、运行 SQL、维护当前文件路径。
 *          左侧导航（已打开文件列表）由 FileWidget 负责，二者通过信号槽协作。
 * @author YuzhSong
 * @date 2026-04-26
 */

#pragma once

#include <QToolBar>
#include <QToolButton>
#include <QWidget>
#include <QMap>

class MainWindow;
class QPushButton;
class SqlEditor;

/**
 * @class EditorWidget
 * @brief SQL 编辑器组件
 * @details 该类只关注“当前编辑文件”的编辑与执行流程，不包含左侧文件导航 UI。
 *          通过 fileOpened/currentFileChanged 通知外部更新 FileWidget。
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
    void reloadEditorSettingsFromLocal();

    /**
     * @brief 执行 SQL 语句入口
     * @details 当前仍只执行当前编辑区 SQL，不处理其他已打开文件。
     * @author YuzhSong
     * @param sql 要执行的 SQL 文本
     */
    void executeSql(const QString& sql);

    void refreshTheme();

public slots:
    /**
     * @brief 切换到指定文件
     * @details 读取文件内容并显示到当前 SqlEditor，更新 currentFilePath 并发出 currentFileChanged。
     * @author YuzhSong
     * @param filePath 目标文件完整路径
     */
    void switchToFile(const QString& fileKey);

    /**
     * @brief 按文件 key 关闭已打开文件
     * @details 支持“保存/不保存/取消”三按钮确认，具体保存逻辑仍复用现有保存流程
     * @param fileKey 待关闭文件唯一标识
     * @author YuzhSong
     */
    void closeFile(const QString& fileKey);

signals:
    /**
     * @brief 文件打开成功信号
     * @details 用于通知 FileWidget 将文件加入“已打开文件”列表。
     * @author YuzhSong
     * @param filePath 打开成功的文件完整路径
     */
    void fileOpened(const QString& fileKey, const QString& displayName, bool modified);

    /**
     * @brief 当前文件变化信号
     * @details 用于通知 FileWidget 更新选中项。
     * @author YuzhSong
     * @param filePath 当前编辑文件完整路径
     */
    void currentFileChanged(const QString& fileKey);

    /**
     * @brief 当前文件未保存状态变化信号
     * @param fileKey 文件唯一标识
     * @param modified 是否未保存
     * @author YuzhSong
     */
    void fileModifiedStateChanged(const QString& fileKey, bool modified);

    /**
     * @brief 文件 key 变更信号（未落盘另存为后用于同步 FileWidget）
     * @param oldFileKey 旧文件 key
     * @param newFileKey 新文件 key
     * @param newDisplayName 新显示名
     * @param modified 新未保存状态
     * @author YuzhSong
     */
    void fileKeyChanged(const QString& oldFileKey, const QString& newFileKey, const QString& newDisplayName, bool modified);

    /**
     * @brief 文件关闭完成信号
     * @param fileKey 已关闭文件 key
     * @author YuzhSong
     */
    void fileClosed(const QString& fileKey);

    /**
     * @brief 请求切换左侧目录面板显示状态
     * @details EditorWidget 不直接控制 DirectoryWidget 或外层 splitter，仅发起请求保持解耦。
     * @author YuzhSong
     */
    void toggleDirectoryRequested();

    /**
     * @brief SQL 已发送信号
     * @details 用于通知 TerminalWidget 记录已执行的 SQL 命令。
     * @author NAPH130
     * @param sql 已发送的 SQL 文本
     */
    void sqlExecuted(const QString& sql);

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
    void applyEditorSettingsFromLocal();

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
    bool confirmDangerousSqlIfNeeded(const QString &sql) const;

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

    /**
     * @struct OpenFileInfo
     * @brief 已打开文件状态结构
     * @details
     * 1. 负责记录每个文件独立的编辑状态，支持多文件切换与关闭确认。
     * 2. 不负责具体列表渲染（由 FileWidget 负责）。
     * 3. 通过 OpePanelWidget 的信号槽编排与 FileWidget 解耦。
     * @author YuzhSong
     */
    struct OpenFileInfo {
        QString fileKey;       ///< 文件唯一标识：已落盘用规范化绝对路径，未落盘用临时 untitled key
        QString displayName;   ///< 文件显示名：用于 FileWidget 与标题展示
        QString filePath;      ///< 真实落盘路径；未落盘文件为空
        QString content;       ///< 当前文件内容快照
        bool isModified;       ///< 是否存在未保存修改
        bool isUntitled;       ///< 是否为新建未落盘文件
    };

    /**
     * @brief 生成未落盘文件唯一 key
     * @return 形如 untitled://N 的临时 key
     * @author YuzhSong
     */
    QString generateUntitledFileKey();

    /**
     * @brief 规范化 SQL 文件名（自动补全 .sql）
     * @param fileName 用户输入文件名
     * @return 规范化后文件名
     * @author YuzhSong
     */
    QString ensureSqlSuffix(const QString& fileName) const;

    /**
     * @brief 统一创建未落盘 SQL 文件状态并切换到该文件
     * @param displayName 用户输入并规范化后的显示名
     * @author YuzhSong
     */
    void createUntitledFile(const QString& displayName);

    /**
     * @brief 将当前编辑器内容回写到当前文件状态
     * @details 切换文件前必须先保存当前编辑区内容，避免状态丢失
     * @author YuzhSong
     */
    void syncEditorToCurrentFileState();

    /**
     * @brief 将目标文件状态加载到编辑区并刷新标题/状态
     * @param fileKey 目标文件 key
     * @author YuzhSong
     */
    void loadFileStateToEditor(const QString& fileKey);

    /**
     * @brief 更新编辑区顶部文件名显示（未保存状态带 *）
     * @author YuzhSong
     */
    void refreshCurrentFileTitle();

    /**
     * @brief 设置当前文件修改状态并通知 FileWidget
     * @param fileKey 文件 key
     * @param modified 是否未保存
     * @author YuzhSong
     */
    void setFileModified(const QString& fileKey, bool modified);

    /**
     * @brief 保存指定文件状态（用于关闭流程中的“保存”）
     * @param fileKey 文件 key
     * @return 成功返回 true；失败或用户取消返回 false
     * @author YuzhSong
     */
    bool saveFileByKey(const QString& fileKey);

    /**
     * @brief 执行指定文件的另存为流程并更新 key/path
     * @param fileKey 文件 key
     * @return 成功返回 true；失败或用户取消返回 false
     * @author YuzhSong
     */
    bool saveFileAsByKey(const QString& fileKey);

    /**
     * @brief 执行关闭动作（不含弹窗决策）
     * @param fileKey 文件 key
     * @author YuzhSong
     */
    void closeFileInternal(const QString& fileKey);

private:
    MainWindow* mainWindow;            ///< 主窗口引用
    SqlEditor* sqlEditor;              ///< SQL 编辑器
    QToolBar* toolbar;                 ///< 顶部工具栏
    QToolButton* btnNew;               ///< 新建按钮
    QToolButton* btnOpen;              ///< 打开按钮
    QToolButton* btnSave;              ///< 保存按钮
    QToolButton* btnRun;               ///< 运行按钮
    QString currentFilePath;           ///< 当前编辑文件完整路径
    QString currentFileKey;            ///< 当前编辑文件唯一 key（未落盘文件不依赖真实路径）
    QMap<QString, OpenFileInfo> openedFiles;  ///< 已打开文件状态表，key 为 fileKey
    int untitledFileCounter;           ///< 未落盘文件自增计数器，保证临时 key 唯一
    bool isInternalContentLoading;     ///< 标记是否处于程序加载文本阶段，避免误判为用户修改
};

