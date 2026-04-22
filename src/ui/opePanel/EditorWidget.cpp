/**
 * @file EditorWidget.cpp
 * @brief SQL 编辑器组件实现文件
 * @details 实现 SQL 编辑、文件操作（新建/打开/保存/另存为）和 SQL 执行功能。
 *          集成 SqlEditor 提供行号显示、语法高亮和自动缩进。
 *          所有文件操作均包含错误处理，Run 按钮支持快捷键 Ctrl+Enter。
 * @author YuzhSong
 * @date 2026-04-09
 */

#include "EditorWidget.h"
#include "mainwindow.h"
#include "sqlEditor.h"

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param mainWindow 主窗口指针
 * @param parent 父部件指针
 */
EditorWidget::EditorWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , sqlEditor(nullptr)
    , toolbar(nullptr)
    , btnNew(nullptr)
    , btnOpen(nullptr)
    , btnSave(nullptr)
    , btnRun(nullptr)
{
    // 初始化 UI 布局
    initUI();

    // 设置编辑器样式和占位提示
    setupEditor();

    // 设置工具栏和按钮
    setupToolbar();

    // 初始化信号槽连接
    initConnections();

    // 输出初始化日志
    qDebug() << QString("[%1][INFO] EditorWidget 初始化完成").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

/**
 * @brief 析构函数
 * @author YuzhSong
 */
EditorWidget::~EditorWidget()
{
    // Qt 会自动清理子对象，此处无需额外清理
    qDebug() << QString("[%1][INFO] EditorWidget 析构").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

/**
 * @brief 获取主窗口指针
 * @author YuzhSong
 * @return 主窗口指针
 */
MainWindow* EditorWidget::getMainWindow() const
{
    return mainWindow;
}

/**
 * @brief 初始化 UI 布局
 * @details 创建垂直布局，从上到下依次为：工具栏、编辑区
 * @author YuzhSong
 */
void EditorWidget::initUI()
{
    // 创建垂直布局管理器
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 设置布局边距和间距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建工具栏（苹果风格：圆角、柔和背景、内边距）
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);  // 按钮样式：仅显示文本
    toolbar->setMovable(false);                           // 禁止移动工具栏
    toolbar->setIconSize(QSize(16, 16));                  // 图标大小
    toolbar->setFixedHeight(44);                          // 固定高度，类似 macOS 标题栏
    
    // 设置工具栏样式（深灰色背景 + 白色文字 + 圆角）
    toolbar->setStyleSheet(
        "QToolBar {"
        "    background-color: #2B2B2B;"
        "    border: none;"
        "    border-bottom: 1px solid #1E1E1E;"
        "    border-radius: 8px;"
        "    padding: 4px;"
        "    spacing: 4px;"
        "}"
        "QToolBar QToolButton {"
        "    background-color: transparent;"
        "    border: 1px solid transparent;"
        "    border-radius: 3px;"
        "    padding: 4px 8px;"
        "    color: #FFFFFF;"
        "}"
        "QToolBar QToolButton:hover {"
        "    background-color: #3E3E3E;"
        "    border: 1px solid #4A4A4A;"
        "}"
        "QToolBar QToolButton:pressed {"
        "    background-color: #4A4A4A;"
        "}"
        "QToolBar QToolButton#btnRun {"
        "    background-color: #4CAF50;"
        "    border: 1px solid #4CAF50;"
        "    color: #FFFFFF;"
        "}"
        "QToolBar QToolButton#btnRun:hover {"
        "    background-color: #45A049;"
        "    border: 1px solid #45A049;"
        "}"
        "QToolBar QToolButton#btnRun:pressed {"
        "    background-color: #3D8B40;"
        "}"
        "QToolBar::separator {"
        "    width: 1px;"
        "    background-color: #3E3E3E;"
        "    margin: 4px 2px;"
        "}"
    );

    // 创建 SQL 编辑区（使用增强的 SqlEditor）
    sqlEditor = new SqlEditor(this);

    // 将工具栏和编辑区添加到布局中
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(sqlEditor, 1);  // 编辑区占据剩余所有空间

    // 设置当前 widget 的布局
    setLayout(mainLayout);
}

/**
 * @brief 初始化信号槽连接
 * @details 连接按钮点击事件和快捷键到对应的槽函数
 * @author YuzhSong
 */
void EditorWidget::initConnections()
{
    // 连接按钮点击事件到槽函数
    connect(btnNew, &QToolButton::clicked, this, &EditorWidget::onNewFile);
    connect(btnOpen, &QToolButton::clicked, this, &EditorWidget::onOpenFile);
    connect(btnSave, &QToolButton::clicked, this, &EditorWidget::onSaveFile);
    connect(btnRun, &QToolButton::clicked, this, &EditorWidget::onRunSql);

    // 设置 Run 按钮快捷键为 Ctrl+Enter
    QShortcut* runShortcut = new QShortcut(QKeySequence("Ctrl+Return"), this);
    connect(runShortcut, &QShortcut::activated, this, &EditorWidget::onRunSql);

    // 额外：Ctrl+S 保存快捷键
    QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(saveShortcut, &QShortcut::activated, this, &EditorWidget::onSaveFile);

    // 额外：Ctrl+O 打开文件快捷键
    QShortcut* openShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(openShortcut, &QShortcut::activated, this, &EditorWidget::onOpenFile);
}

/**
 * @brief 设置编辑器样式和占位提示
 * @details 配置 SqlEditor 的字体、默认提示文本等
 * @author YuzhSong
 */
void EditorWidget::setupEditor()
{
    // SqlEditor 已在构造函数中初始化行号和高亮器
    // 此处仅设置占位提示文本
    sqlEditor->setPlaceholderText("\n-- Enter your SQL here\n-- 例如：SELECT * FROM table_name;");
}

/**
 * @brief 设置工具栏和按钮
 * @details 创建工具栏并添加新建/打开/保存/运行按钮
 * @author YuzhSong
 */
void EditorWidget::setupToolbar()
{
    // 创建新建按钮
    btnNew = new QToolButton(this);
    btnNew->setObjectName("btnNew");
    btnNew->setText(tr("新建"));
    btnNew->setToolTip(tr("新建SQL文本(清空编辑器)"));

    // 创建打开按钮
    btnOpen = new QToolButton(this);
    btnOpen->setObjectName("btnOpen");
    btnOpen->setText(tr("打开"));
    btnOpen->setToolTip(tr("打开.sql文件(Ctrl+O)"));

    // 创建保存按钮
    btnSave = new QToolButton(this);
    btnSave->setObjectName("btnSave");
    btnSave->setText(tr("保存"));
    btnSave->setToolTip(tr("保存文件(Ctrl+S)"));

    // 创建运行按钮
    btnRun = new QToolButton(this);
    btnRun->setObjectName("btnRun");
    btnRun->setText(tr(" ▶ "));
    btnRun->setToolTip(tr("执行SQL(Ctrl+Enter)"));

    // 将按钮添加到工具栏
    toolbar->addWidget(btnNew);
    toolbar->addWidget(btnOpen);
    toolbar->addWidget(btnSave);

    // 添加分隔符
    toolbar->addSeparator();

    toolbar->addWidget(btnRun);
}

/**
 * @brief 新建 SQL 文本槽函数
 * @details 清空编辑器内容，重置当前文件路径
 * @author YuzhSong
 */
void EditorWidget::onNewFile()
{
    // 检查编辑器是否有内容
    if (!sqlEditor->toPlainText().isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("新建文件"),
            tr("当前编辑器有内容，是否清空？"),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::No) {
            return;  // 用户取消操作
        }
    }

    // 清空编辑器内容
    sqlEditor->clear();

    // 重置当前文件路径
    currentFilePath.clear();

    // 更新窗口标题
    updateWindowTitle(tr("未命名"));

    qDebug() << QString("[%1][INFO] EditorWidget::onNewFile - 已新建 SQL 文本").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

/**
 * @brief 打开 .sql 文件槽函数
 * @details 弹出文件选择对话框，读取选中的 .sql 文件内容到编辑器
 * @author YuzhSong
 */
void EditorWidget::onOpenFile()
{
    // 弹出文件选择对话框，限制只显示 .sql 文件
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开 SQL 文件"),
        QDir::homePath(),
        tr("SQL Files (*.sql);;All Files (*)")
    );

    // 如果用户选择了文件
    if (!filePath.isEmpty()) {
        // 尝试加载文件
        if (loadFile(filePath)) {
            qDebug() << QString("[%1][INFO] EditorWidget::onOpenFile - 成功打开文件: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath);
        } else {
            // 加载失败，显示错误提示
            QMessageBox::critical(
                this,
                tr("打开文件失败"),
                tr("无法打开文件:\n%1").arg(filePath)
            );
            qWarning() << QString("[%1][ERROR] EditorWidget::onOpenFile - 打开文件失败: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath);
        }
    }
}

/**
 * @brief 保存文件槽函数
 * @details 如果当前文件已有路径，直接保存；否则弹出另存为对话框
 * @author YuzhSong
 */
void EditorWidget::onSaveFile()
{
    // 如果当前文件已有路径，直接保存
    if (!currentFilePath.isEmpty()) {
        if (saveFile(currentFilePath)) {
            qDebug() << QString("[%1][INFO] EditorWidget::onSaveFile - 成功保存文件: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), currentFilePath);
        } else {
            QMessageBox::critical(
                this,
                tr("保存文件失败"),
                tr("无法保存文件:\n%1").arg(currentFilePath)
            );
            qWarning() << QString("[%1][ERROR] EditorWidget::onSaveFile - 保存文件失败: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), currentFilePath);
        }
    } else {
        // 否则弹出另存为对话框
        onSaveFileAs();
    }
}

/**
 * @brief 另存为文件槽函数
 * @details 弹出另存为对话框，保存当前编辑内容
 * @author YuzhSong
 */
void EditorWidget::onSaveFileAs()
{
    // 弹出另存为对话框
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("另存为 SQL 文件"),
        QDir::homePath() + "/untitled.sql",
        tr("SQL Files (*.sql);;All Files (*)")
    );

    // 如果用户指定了文件路径
    if (!filePath.isEmpty()) {
        // 确保文件扩展名为 .sql
        if (!filePath.endsWith(".sql", Qt::CaseInsensitive)) {
            filePath += ".sql";
        }

        // 尝试保存文件
        if (saveFile(filePath)) {
            // 更新当前文件路径
            currentFilePath = filePath;

            // 更新窗口标题
            updateWindowTitle(QFileInfo(filePath).fileName());

            qDebug() << QString("[%1][INFO] EditorWidget::onSaveFileAs - 成功另存为文件: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath);
        } else {
            QMessageBox::critical(
                this,
                tr("保存文件失败"),
                tr("无法保存文件:\n%1").arg(filePath)
            );
            qWarning() << QString("[%1][ERROR] EditorWidget::onSaveFileAs - 另存为文件失败: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath);
        }
    }
}

/**
 * @brief 运行 SQL 槽函数
 * @details 获取当前编辑器中的 SQL 文本，调用 executeSql 方法执行
 * @author YuzhSong
 */
void EditorWidget::onRunSql()
{
    // 获取编辑器中的 SQL 文本
    QString sql = sqlEditor->toPlainText().trimmed();

    // 检查 SQL 是否为空
    if (sql.isEmpty()) {
        QMessageBox::information(
            this,
            tr("提示"),
            tr("请先输入 SQL 语句")
        );
        return;
    }

    // 调用 executeSql 方法执行 SQL
    executeSql(sql);
}

/**
 * @brief 执行 SQL 语句接口
 * @details 当前仅做调试输出，后续用于连接 DBMS
 * @author YuzhSong
 * @param sql 要执行的 SQL 语句
 */
void EditorWidget::executeSql(const QString& sql)
{
    // 当前实现：仅输出 SQL 到调试控制台
    qDebug() << QString("[%1][DEBUG] EditorWidget::executeSql - 执行 SQL: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), sql);

    // TODO: 后续在此处添加与 DBMS 的通信逻辑
    // 示例：
    // mainWindow->getNetSender()->sendSql(sql);
    // 或者调用其他网络模块方法
}

/**
 * @brief 从文件读取内容到编辑器
 * @author YuzhSong
 * @param filePath 文件路径
 * @return 是否读取成功
 */
bool EditorWidget::loadFile(const QString& filePath)
{
    // 创建 QFile 对象
    QFile file(filePath);

    // 尝试以只读方式打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << QString("[%1][ERROR] EditorWidget::loadFile - 无法打开文件: %2, 错误: %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath, file.errorString());
        return false;
    }

    // 使用 QTextStream 读取文件内容
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);  // 设置编码为 UTF-8

    QString content = in.readAll();

    // 关闭文件
    file.close();

    // 将内容设置到编辑器
    sqlEditor->setPlainText(content);

    // 更新当前文件路径
    currentFilePath = filePath;

    // 更新窗口标题
    updateWindowTitle(QFileInfo(filePath).fileName());

    return true;
}

/**
 * @brief 保存编辑器内容到文件
 * @author YuzhSong
 * @param filePath 文件路径
 * @return 是否保存成功
 */
bool EditorWidget::saveFile(const QString& filePath)
{
    // 创建 QFile 对象
    QFile file(filePath);

    // 尝试以写入方式打开文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << QString("[%1][ERROR] EditorWidget::saveFile - 无法写入文件: %2, 错误: %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), filePath, file.errorString());
        return false;
    }

    // 使用 QTextStream 写入文件内容
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);  // 设置编码为 UTF-8

    // 获取编辑器内容并写入文件
    out << sqlEditor->toPlainText();

    // 关闭文件
    file.close();

    // 更新窗口标题
    updateWindowTitle(QFileInfo(filePath).fileName());

    return true;
}

/**
 * @brief 更新窗口标题显示当前文件名
 * @author YuzhSong
 * @param fileName 当前文件名
 */
void EditorWidget::updateWindowTitle(const QString& fileName)
{
    // 输出调试信息
    qDebug() << QString("[%1][INFO] EditorWidget::updateWindowTitle - 当前文件: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), fileName);

    // 如果需要更新主窗口标题，可以调用：
    // mainWindow->setWindowTitle(tr("simpleDBMS-Client - %1").arg(fileName));
}
