/**
 * @file EditorWidget.cpp
 * @brief SQL 编辑器主组件实现文件
 * @details 负责编辑、文件打开/保存/另存为、运行当前 SQL。
 *          左侧导航文件列表由 FileWidget 管理，本类通过信号通知外部协调层更新左侧列表。
 * @author YuzhSong
 * @date 2026-04-26
 */

#include "EditorWidget.h"

#include "SqlEditor.h"
#include "mainwindow.h"
#include "network/NetworkManager.h"
#include "network/NetSender.h"
#include "models/network/NetworkTransferData.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMessageBox>
#include <QSizePolicy>
#include <QShortcut>
#include <QStringConverter>
#include <QTextStream>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param mainWindow 主窗口指针
 * @param parent 父组件指针
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
    initUI();
    setupEditor();
    setupToolbar();
    initConnections();

    qDebug() << QString("[%1][INFO] EditorWidget initialized")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

/**
 * @brief 析构函数
 * @author YuzhSong
 */
EditorWidget::~EditorWidget()
{
    qDebug() << QString("[%1][INFO] EditorWidget destroyed")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
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
 * @brief 初始化整体 UI
 * @author YuzhSong
 */
void EditorWidget::initUI()
{
    // 作者：YuzhSong
    // EditorWidget 外层直接作为主编辑面板，统一圆角和背景层级，不改动 SqlEditor 内部语法高亮与行号逻辑。
    setObjectName("editorWidgetRoot");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    // 作者：YuzhSong
    // 工具栏与编辑区间使用细间隙衔接，保持一体化面板观感同时保留轻微层次分界。
    mainLayout->setSpacing(0);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setFixedHeight(44);
    toolbar->setObjectName("editorToolbar");
    // 作者：YuzhSong
    // 仅统一工具栏视觉样式，不修改新建/打开/保存/运行及目录开关的业务行为。
    toolbar->setStyleSheet(
        "EditorWidget#editorWidgetRoot {"
        "    background-color: #111315;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 14px;"
        "}"
        "QToolBar#editorToolbar {"
        "    background-color: #111315;"
        "    border: none;"
        "    border-top-left-radius: 14px;"
        "    border-top-right-radius: 14px;"
        "    padding: 4px 6px;"
        "    spacing: 4px;"
        "}"
        "QToolBar#editorToolbar QToolButton {"
        "    background-color: #1A1C1E;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 8px;"
        "    padding: 4px 10px;"
        "    color: #F0F0F0;"
        "}"
        "QToolBar#editorToolbar QToolButton:hover {"
        "    background-color: #25292D;"
        "}"
        "QToolBar#editorToolbar QToolButton:pressed {"
        "    background-color: #2D3135;"
        "}"
        "QToolBar#editorToolbar QToolButton#btnRun {"
        "    background-color: #3FB950;"
        "    border: 1px solid #2F9E44;"
        "    color: #FFFFFF;"
        "    font-weight: 600;"
        "}"
        "QToolBar#editorToolbar QToolButton#btnRun:hover {"
        "    background-color: #4AC45C;"
        "    border: 1px solid #3FB950;"
        "}"
        "QToolBar#editorToolbar QToolButton#btnRun:pressed {"
        "    background-color: #2F9E44;"
        "}"
        "QToolBar#editorToolbar::separator {"
        "    width: 1px;"
        "    background-color: #1B1D20;"
        "    margin: 4px 2px;"
        "}"
        "QWidget#editorToolbarSpacer {"
        "    background-color: #111315;"
        "    border: none;"
        "}"
        "QPlainTextEdit#editorSqlInput {"
        "    background-color: #111315;"
        "    border: none;"
        "    border-bottom-left-radius: 14px;"
        "    border-bottom-right-radius: 14px;"
        "}"
    );

    sqlEditor = new SqlEditor(this);
    sqlEditor->setObjectName("editorSqlInput");
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(sqlEditor, 1);
    setLayout(mainLayout);
}

/**
 * @brief 初始化信号槽
 * @author YuzhSong
 */
void EditorWidget::initConnections()
{
    connect(btnNew, &QToolButton::clicked, this, &EditorWidget::onNewFile);
    connect(btnOpen, &QToolButton::clicked, this, &EditorWidget::onOpenFile);
    connect(btnSave, &QToolButton::clicked, this, &EditorWidget::onSaveFile);
    connect(btnRun, &QToolButton::clicked, this, &EditorWidget::onRunSql);

    auto* runShortcut = new QShortcut(QKeySequence("Ctrl+Return"), this);
    connect(runShortcut, &QShortcut::activated, this, &EditorWidget::onRunSql);

    auto* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(saveShortcut, &QShortcut::activated, this, &EditorWidget::onSaveFile);

    auto* openShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(openShortcut, &QShortcut::activated, this, &EditorWidget::onOpenFile);
}

/**
 * @brief 设置编辑器基础显示属性
 * @author YuzhSong
 */
void EditorWidget::setupEditor()
{
    sqlEditor->setPlaceholderText("\n-- Enter your SQL here\n-- 例如：SELECT * FROM table_name;");
}

/**
 * @brief 设置工具栏
 * @author YuzhSong
 */
void EditorWidget::setupToolbar()
{
    btnNew = new QToolButton(this);
    btnNew->setObjectName("btnNew");
    btnNew->setText(tr("新建"));
    btnNew->setToolTip(tr("新建 SQL 文本"));

    btnOpen = new QToolButton(this);
    btnOpen->setObjectName("btnOpen");
    btnOpen->setText(tr("打开"));
    btnOpen->setToolTip(tr("打开 .sql 文件 (Ctrl+O)"));

    btnSave = new QToolButton(this);
    btnSave->setObjectName("btnSave");
    btnSave->setText(tr("保存"));
    btnSave->setToolTip(tr("保存当前文件 (Ctrl+S)"));

    btnRun = new QToolButton(this);
    btnRun->setObjectName("btnRun");
    btnRun->setText(tr("Run"));
    btnRun->setToolTip(tr("执行当前编辑区 SQL (Ctrl+Enter)"));

    toolbar->addWidget(btnNew);
    toolbar->addWidget(btnOpen);
    toolbar->addWidget(btnSave);

    // 作者：YuzhSong
    // 在左侧文件操作按钮与右侧 Run 按钮之间加入弹性间隔，使 Run 固定靠右显示。
    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setObjectName("editorToolbarSpacer");
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(stretchWidget);

    toolbar->addWidget(btnRun);
}

/**
 * @brief 新建文件槽函数
 * @author YuzhSong
 */
void EditorWidget::onNewFile()
{
    if (!sqlEditor->toPlainText().isEmpty()) {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("新建文件"),
            tr("当前编辑区存在内容，是否清空并新建？"),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    sqlEditor->clear();
    currentFilePath.clear();
    updateWindowTitle(tr("未命名"));
}

/**
 * @brief 打开文件槽函数
 * @author YuzhSong
 */
void EditorWidget::onOpenFile()
{
    openFile();
}

/**
 * @brief 保存文件槽函数
 * @author YuzhSong
 */
void EditorWidget::onSaveFile()
{
    saveFile();
}

/**
 * @brief 另存为槽函数
 * @author YuzhSong
 */
void EditorWidget::onSaveFileAs()
{
    saveFileAs();
}

/**
 * @brief 运行 SQL 槽函数
 * @author YuzhSong
 */
void EditorWidget::onRunSql()
{
    runCurrentSql();
}

/**
 * @brief 打开文件流程封装
 * @author YuzhSong
 * @return 成功返回 true，失败返回 false
 */
bool EditorWidget::openFile()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开 SQL 文件"),
        QDir::homePath(),
        tr("SQL Files (*.sql);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return false;
    }

    const QString normalizedFilePath = normalizeFilePath(filePath);
    if (!loadFile(normalizedFilePath)) {
        QMessageBox::critical(this, tr("打开文件失败"), tr("无法打开文件：\n%1").arg(normalizedFilePath));
        return false;
    }

    currentFilePath = normalizedFilePath;
    updateWindowTitle(QFileInfo(currentFilePath).fileName());

    // 打开成功后通知目录组件：
    // 1) 将文件加入"已打开文件"
    // 2) 将其设置为当前选中项
    emit fileOpened(currentFilePath);
    emit currentFileChanged(currentFilePath);
    return true;
}

/**
 * @brief 保存文件流程封装
 * @author YuzhSong
 * @return 成功返回 true，失败返回 false
 */
bool EditorWidget::saveFile()
{
    if (currentFilePath.isEmpty()) {
        return saveFileAs();
    }

    if (!saveFileToPath(currentFilePath)) {
        QMessageBox::critical(this, tr("保存失败"), tr("无法保存文件：\n%1").arg(currentFilePath));
        return false;
    }

    updateWindowTitle(QFileInfo(currentFilePath).fileName());
    emit currentFileChanged(currentFilePath);
    return true;
}

/**
 * @brief 另存为流程封装
 * @author YuzhSong
 * @return 成功返回 true，失败返回 false
 */
bool EditorWidget::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("另存为 SQL 文件"),
        QDir::homePath() + "/untitled.sql",
        tr("SQL Files (*.sql);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return false;
    }

    if (!filePath.endsWith(".sql", Qt::CaseInsensitive)) {
        filePath += ".sql";
    }

    const QString normalizedFilePath = normalizeFilePath(filePath);
    if (!saveFileToPath(normalizedFilePath)) {
        QMessageBox::critical(this, tr("保存失败"), tr("无法保存文件：\n%1").arg(normalizedFilePath));
        return false;
    }

    currentFilePath = normalizedFilePath;
    updateWindowTitle(QFileInfo(currentFilePath).fileName());

    // 另存为后的文件也应纳入"已打开文件"，并同步当前选中状态。
    emit fileOpened(currentFilePath);
    emit currentFileChanged(currentFilePath);
    return true;
}

/**
 * @brief 切换到指定文件
 * @author YuzhSong
 * @param filePath 目标文件完整路径
 */
void EditorWidget::switchToFile(const QString& filePath)
{
    const QString normalizedFilePath = normalizeFilePath(filePath);
    if (normalizedFilePath.isEmpty()) {
        return;
    }

    if (!loadFile(normalizedFilePath)) {
        QMessageBox::critical(this, tr("打开文件失败"), tr("无法打开文件：\n%1").arg(normalizedFilePath));
        qWarning() << QString("[%1][WARN] Failed to switch file: %2")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), normalizedFilePath);
        return;
    }

    currentFilePath = normalizedFilePath;
    updateWindowTitle(QFileInfo(currentFilePath).fileName());
    emit currentFileChanged(currentFilePath);
}

/**
 * @brief 运行当前编辑区 SQL 流程封装
 * @author YuzhSong
 */
void EditorWidget::runCurrentSql()
{
    const QString sql = sqlEditor->toPlainText().trimmed();
    if (sql.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先输入 SQL 语句"));
        return;
    }

    executeSql(sql);
}

/**
 * @brief 执行 SQL 语句入口
 * @details 通过 MainWindow → NetworkManager → NetSender 链式获取发送器，
 *          将 SQL 封装为 NetworkTransferData 后通过 NetSender 发送至服务端。
 * @author YuzhSong
 * @param sql 要执行的 SQL 文本
 */
void EditorWidget::executeSql(const QString& sql)
{
    if (mainWindow == nullptr) {
        qWarning() << QString("[%1][ERROR] MainWindow is null, cannot send SQL.")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        return;
    }

    NetworkManager* networkManager = mainWindow->getNetworkManager();
    if (networkManager == nullptr) {
        qWarning() << QString("[%1][ERROR] NetworkManager is null, cannot send SQL.")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        return;
    }

    NetSender* netSender = networkManager->getNetSender();
    if (netSender == nullptr) {
        qWarning() << QString("[%1][ERROR] NetSender is null, cannot send SQL.")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        return;
    }

    std::shared_ptr<asio::ip::tcp::socket> socket = networkManager->getSocket();
    if (socket == nullptr || !socket->is_open()) {
        qWarning() << QString("[%1][ERROR] Socket is not connected, cannot send SQL.")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        return;
    }

    NetworkTransferData data;
    data.setType(NetworkTransferData::SQL_EXEC_REQUEST);
    data.setSql(sql.toStdString());

    netSender->send(socket, data.toJson());

    emit sqlExecuted(sql);

    qDebug() << QString("[%1][DEBUG] SQL sent to server: %2")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), sql);
}

/**
 * @brief 从文件读取内容到编辑器
 * @author YuzhSong
 * @param filePath 文件完整路径
 * @return 成功返回 true，失败返回 false
 */
bool EditorWidget::loadFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << QString("[%1][ERROR] Cannot open file: %2, error: %3")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                               filePath,
                               file.errorString());
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    const QString content = in.readAll();
    file.close();

    sqlEditor->setPlainText(content);
    return true;
}

/**
 * @brief 保存编辑器内容到文件
 * @author YuzhSong
 * @param filePath 文件完整路径
 * @return 成功返回 true，失败返回 false
 */
bool EditorWidget::saveFileToPath(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << QString("[%1][ERROR] Cannot write file: %2, error: %3")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                               filePath,
                               file.errorString());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << sqlEditor->toPlainText();
    file.close();
    return true;
}

/**
 * @brief 更新窗口标题显示当前文件名
 * @details 目前保留日志输出；如需显示主窗标题可在此接入 MainWindow::setWindowTitle。
 * @author YuzhSong
 * @param fileName 当前文件名
 */
void EditorWidget::updateWindowTitle(const QString& fileName)
{
    qDebug() << QString("[%1][INFO] Current file: %2")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), fileName);
}

/**
 * @brief 规范化路径
 * @author YuzhSong
 * @param filePath 原始路径
 * @return 规范化后的绝对路径
 */
QString EditorWidget::normalizeFilePath(const QString& filePath) const
{
    if (filePath.isEmpty()) {
        return QString();
    }
    return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
}
