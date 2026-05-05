/**
 * @file EditorWidget.cpp
 * @brief SQL 编辑器主组件实现文件
 * @details 负责编辑、文件打开/保存/另存为、运行当前 SQL。
 *          左侧导航由 DirectoryWidget 管理，本类通过信号通知外部协调层更新左侧列表。
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
#include <QPushButton>
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
    , btnToggleDirectory(nullptr)
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
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setFixedHeight(44);
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
        "QToolBar QPushButton#btnToggleDirectory {"
        "    background-color: #3A3A3A;"
        "    border: 1px solid #4A4A4A;"
        "    border-radius: 3px;"
        "    padding: 4px 10px;"
        "    color: #FFFFFF;"
        "}"
        "QToolBar QPushButton#btnToggleDirectory:hover {"
        "    background-color: #4A4A4A;"
        "}"
        "QToolBar::separator {"
        "    width: 1px;"
        "    background-color: #3E3E3E;"
        "    margin: 4px 2px;"
        "}"
    );

    sqlEditor = new SqlEditor(this);
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

    // 该按钮只发请求，不直接操作目录组件或 splitter，避免跨组件强耦合。
    connect(btnToggleDirectory, &QPushButton::clicked, this, [this]() {
        emit toggleDirectoryRequested();
    });

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

    btnToggleDirectory = new QPushButton(this);
    btnToggleDirectory->setObjectName("btnToggleDirectory");
    btnToggleDirectory->setText(tr("侧栏"));
    btnToggleDirectory->setToolTip(tr("展开/收起左侧目录面板"));

    toolbar->addWidget(btnNew);
    toolbar->addWidget(btnOpen);
    toolbar->addWidget(btnSave);
    toolbar->addSeparator();
    toolbar->addWidget(btnToggleDirectory);
    toolbar->addSeparator();
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