/**
 * @file EditorWidget.cpp
 * @brief SQL 编辑器主组件实现文件
 * @details 负责编辑、文件打开/保存/另存为、运行当前 SQL，以及多文件状态管理与关闭确认
 * @author YuzhSong
 * @date 2026-05-10
 * @module ui/opePanel
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
#include <QInputDialog>
#include <QKeySequence>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QStringConverter>
#include <QTextStream>
#include <QVBoxLayout>

EditorWidget::EditorWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , sqlEditor(nullptr)
    , toolbar(nullptr)
    , btnNew(nullptr)
    , btnOpen(nullptr)
    , btnSave(nullptr)
    , btnRun(nullptr)
    , untitledFileCounter(1)
    , isInternalContentLoading(false)
{
    initUI();
    setupEditor();
    setupToolbar();
    initConnections();
}

EditorWidget::~EditorWidget() = default;

MainWindow* EditorWidget::getMainWindow() const
{
    return mainWindow;
}

void EditorWidget::initUI()
{
    setObjectName("editorWidgetRoot");
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setFixedHeight(44);
    toolbar->setObjectName("editorToolbar");
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
        "QToolBar#editorToolbar QToolButton#btnRun {"
        "    background-color: #3FB950;"
        "    border: 1px solid #2F9E44;"
        "    color: #FFFFFF;"
        "    font-weight: 600;"
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

    // 作者：YuzhSong
    // 仅监听文本变化更新文件状态，不修改 SqlEditor 自动补全、语法高亮、行号等内部逻辑。
    connect(sqlEditor, &QPlainTextEdit::textChanged, this, [this]() {
        if (isInternalContentLoading || currentFileKey.isEmpty() || !openedFiles.contains(currentFileKey)) {
            return;
        }
        OpenFileInfo& currentInfo = openedFiles[currentFileKey];
        currentInfo.content = sqlEditor->toPlainText();
        setFileModified(currentFileKey, true);
    });
}

void EditorWidget::setupEditor()
{
    sqlEditor->setPlaceholderText("\n-- Enter your SQL here\n-- 例如：SELECT * FROM table_name;");
}

void EditorWidget::setupToolbar()
{
    btnNew = new QToolButton(this);
    btnNew->setObjectName("btnNew");
    btnNew->setText(tr("新建"));
    btnOpen = new QToolButton(this);
    btnOpen->setObjectName("btnOpen");
    btnOpen->setText(tr("打开"));
    btnSave = new QToolButton(this);
    btnSave->setObjectName("btnSave");
    btnSave->setText(tr("保存"));
    btnRun = new QToolButton(this);
    btnRun->setObjectName("btnRun");
    btnRun->setText(tr("Run"));

    toolbar->addWidget(btnNew);
    toolbar->addWidget(btnOpen);
    toolbar->addWidget(btnSave);
    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setObjectName("editorToolbarSpacer");
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(stretchWidget);
    toolbar->addWidget(btnRun);
}

void EditorWidget::onNewFile()
{
    bool ok = false;
    const QString inputName = QInputDialog::getText(
        this, tr("新建 SQL 文件"), tr("请输入文件名："), QLineEdit::Normal, tr("Untitled.sql"), &ok);
    if (!ok) {
        return;
    }
    const QString trimmedName = inputName.trimmed();
    if (trimmedName.isEmpty()) {
        QMessageBox::warning(this, tr("文件名无效"), tr("文件名不能为空。"));
        return;
    }
    createUntitledFile(ensureSqlSuffix(trimmedName));
}

void EditorWidget::onOpenFile() { openFile(); }
void EditorWidget::onSaveFile() { saveFile(); }
void EditorWidget::onSaveFileAs() { saveFileAs(); }
void EditorWidget::onRunSql() { runCurrentSql(); }

bool EditorWidget::openFile()
{
    const QString filePath = QFileDialog::getOpenFileName(this, tr("打开 SQL 文件"), QDir::homePath(), tr("SQL Files (*.sql);;All Files (*)"));
    if (filePath.isEmpty()) {
        return false;
    }
    const QString normalizedFilePath = normalizeFilePath(filePath);
    if (openedFiles.contains(normalizedFilePath)) {
        switchToFile(normalizedFilePath);
        return true;
    }
    // 作者：YuzhSong
    // 打开新文件前先回写当前编辑区状态，避免切换后覆盖已有打开文件的内容快照。
    syncEditorToCurrentFileState();
    if (!loadFile(normalizedFilePath)) {
        QMessageBox::critical(this, tr("打开文件失败"), tr("无法打开文件：\n%1").arg(normalizedFilePath));
        return false;
    }
    OpenFileInfo openInfo{normalizedFilePath, QFileInfo(normalizedFilePath).fileName(), normalizedFilePath, sqlEditor->toPlainText(), false, false};
    openedFiles.insert(openInfo.fileKey, openInfo);
    currentFileKey = openInfo.fileKey;
    currentFilePath = openInfo.filePath;
    refreshCurrentFileTitle();

    emit fileOpened(openInfo.fileKey, openInfo.displayName, openInfo.isModified);
    emit currentFileChanged(openInfo.fileKey);
    return true;
}

bool EditorWidget::saveFile() { return saveFileByKey(currentFileKey); }
bool EditorWidget::saveFileAs() { return saveFileAsByKey(currentFileKey); }

void EditorWidget::switchToFile(const QString& fileKey)
{
    if (fileKey.isEmpty() || !openedFiles.contains(fileKey) || currentFileKey == fileKey) {
        return;
    }
    syncEditorToCurrentFileState();
    loadFileStateToEditor(fileKey);
    emit currentFileChanged(fileKey);
}

void EditorWidget::closeFile(const QString& fileKey)
{
    if (fileKey.isEmpty() || !openedFiles.contains(fileKey)) {
        return;
    }
    const OpenFileInfo info = openedFiles.value(fileKey);
    if (!info.isModified) {
        closeFileInternal(fileKey);
        return;
    }

    QMessageBox confirmBox(this);
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setWindowTitle(tr("关闭文件"));
    confirmBox.setText(tr("是否保存对此文件的修改？"));
    QPushButton* saveButton = confirmBox.addButton(tr("保存"), QMessageBox::AcceptRole);
    QPushButton* discardButton = confirmBox.addButton(tr("不保存"), QMessageBox::DestructiveRole);
    QPushButton* cancelButton = confirmBox.addButton(tr("取消"), QMessageBox::RejectRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == cancelButton) {
        return;
    }
    if (confirmBox.clickedButton() == saveButton && !saveFileByKey(fileKey)) {
        // 作者：YuzhSong
        // 保存失败或取消另存为时不能关闭文件，确保未保存内容不丢失。
        return;
    }
    if (confirmBox.clickedButton() == saveButton || confirmBox.clickedButton() == discardButton) {
        closeFileInternal(fileKey);
    }
}

void EditorWidget::runCurrentSql()
{
    const QString sql = sqlEditor->toPlainText().trimmed();
    if (sql.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先输入 SQL 语句"));
        return;
    }
    executeSql(sql);
}

void EditorWidget::executeSql(const QString& sql)
{
    if (mainWindow == nullptr) return;
    NetworkManager* networkManager = mainWindow->getNetworkManager();
    if (networkManager == nullptr) return;
    NetSender* netSender = networkManager->getNetSender();
    if (netSender == nullptr) return;
    std::shared_ptr<asio::ip::tcp::socket> socket = networkManager->getSocket();
    if (socket == nullptr || !socket->is_open()) return;

    NetworkTransferData data;
    data.setType(NetworkTransferData::SQL_EXEC_REQUEST);
    data.setSql(sql.toStdString());
    netSender->send(socket, data.toJson());
    emit sqlExecuted(sql);
}

bool EditorWidget::loadFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    const QString content = in.readAll();
    file.close();

    isInternalContentLoading = true;
    sqlEditor->setPlainText(content);
    isInternalContentLoading = false;
    return true;
}

bool EditorWidget::saveFileToPath(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    QString contentToWrite = sqlEditor->toPlainText();
    if (!currentFileKey.isEmpty() && openedFiles.contains(currentFileKey)) {
        contentToWrite = openedFiles.value(currentFileKey).content;
    }
    out << contentToWrite;
    file.close();
    return true;
}

void EditorWidget::updateWindowTitle(const QString& fileName)
{
    qDebug() << QString("[%1][INFO] Current file: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), fileName);
}

QString EditorWidget::normalizeFilePath(const QString& filePath) const
{
    if (filePath.isEmpty()) return QString();
    return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
}

QString EditorWidget::generateUntitledFileKey()
{
    return QString("untitled://%1").arg(untitledFileCounter++);
}

QString EditorWidget::ensureSqlSuffix(const QString& fileName) const
{
    return fileName.endsWith(".sql", Qt::CaseInsensitive) ? fileName : fileName + ".sql";
}

void EditorWidget::createUntitledFile(const QString& displayName)
{
    syncEditorToCurrentFileState();
    OpenFileInfo info{generateUntitledFileKey(), displayName, QString(), QString(), true, true};
    openedFiles.insert(info.fileKey, info);
    loadFileStateToEditor(info.fileKey);
    emit fileOpened(info.fileKey, info.displayName, info.isModified);
    emit currentFileChanged(info.fileKey);
}

void EditorWidget::syncEditorToCurrentFileState()
{
    if (currentFileKey.isEmpty() || !openedFiles.contains(currentFileKey)) return;
    openedFiles[currentFileKey].content = sqlEditor->toPlainText();
}

void EditorWidget::loadFileStateToEditor(const QString& fileKey)
{
    if (!openedFiles.contains(fileKey)) return;
    const OpenFileInfo info = openedFiles.value(fileKey);
    isInternalContentLoading = true;
    sqlEditor->setPlainText(info.content);
    isInternalContentLoading = false;
    currentFileKey = fileKey;
    currentFilePath = info.filePath;
    refreshCurrentFileTitle();
}

void EditorWidget::refreshCurrentFileTitle()
{
    if (currentFileKey.isEmpty() || !openedFiles.contains(currentFileKey)) {
        updateWindowTitle(tr("未命名"));
        return;
    }
    const OpenFileInfo info = openedFiles.value(currentFileKey);
    updateWindowTitle(info.isModified ? QString("%1 *").arg(info.displayName) : info.displayName);
}

void EditorWidget::setFileModified(const QString& fileKey, bool modified)
{
    if (!openedFiles.contains(fileKey)) return;
    OpenFileInfo& info = openedFiles[fileKey];
    if (info.isModified == modified) return;
    info.isModified = modified;
    emit fileModifiedStateChanged(fileKey, modified);
    if (fileKey == currentFileKey) refreshCurrentFileTitle();
}

bool EditorWidget::saveFileByKey(const QString& fileKey)
{
    if (fileKey.isEmpty() || !openedFiles.contains(fileKey)) return false;
    if (fileKey == currentFileKey) syncEditorToCurrentFileState();
    OpenFileInfo info = openedFiles.value(fileKey);
    if (info.filePath.isEmpty() || info.isUntitled) return saveFileAsByKey(fileKey);

    const QString path = normalizeFilePath(info.filePath);
    const QString previousKey = currentFileKey;
    const QString previousPath = currentFilePath;
    currentFileKey = fileKey;
    currentFilePath = path;
    if (!saveFileToPath(path)) {
        currentFileKey = previousKey;
        currentFilePath = previousPath;
        QMessageBox::critical(this, tr("保存失败"), tr("无法保存文件：\n%1").arg(path));
        return false;
    }
    currentFileKey = previousKey;
    currentFilePath = previousPath;

    OpenFileInfo& mutableInfo = openedFiles[fileKey];
    mutableInfo.filePath = path;
    mutableInfo.isUntitled = false;
    setFileModified(fileKey, false);
    return true;
}

bool EditorWidget::saveFileAsByKey(const QString& fileKey)
{
    if (fileKey.isEmpty() || !openedFiles.contains(fileKey)) return false;
    if (fileKey == currentFileKey) syncEditorToCurrentFileState();
    const OpenFileInfo info = openedFiles.value(fileKey);

    const QString suggestedName = info.displayName.isEmpty() ? QString("untitled.sql") : info.displayName;
    QString selectedPath = QFileDialog::getSaveFileName(this, tr("另存为 SQL 文件"), QDir::homePath() + "/" + suggestedName, tr("SQL Files (*.sql);;All Files (*)"));
    if (selectedPath.isEmpty()) return false;
    if (!selectedPath.endsWith(".sql", Qt::CaseInsensitive)) selectedPath += ".sql";
    const QString normalizedPath = normalizeFilePath(selectedPath);

    const QString previousKey = currentFileKey;
    const QString previousPath = currentFilePath;
    currentFileKey = fileKey;
    currentFilePath = normalizedPath;
    if (!saveFileToPath(normalizedPath)) {
        currentFileKey = previousKey;
        currentFilePath = previousPath;
        QMessageBox::critical(this, tr("保存失败"), tr("无法保存文件：\n%1").arg(normalizedPath));
        return false;
    }
    currentFileKey = previousKey;
    currentFilePath = previousPath;

    OpenFileInfo mutableInfo = openedFiles.value(fileKey);
    const QString oldKey = mutableInfo.fileKey;
    const QString newKey = normalizedPath;
    mutableInfo.fileKey = newKey;
    mutableInfo.filePath = normalizedPath;
    mutableInfo.displayName = QFileInfo(normalizedPath).fileName();
    mutableInfo.isUntitled = false;
    mutableInfo.isModified = false;
    openedFiles.remove(oldKey);
    openedFiles.insert(newKey, mutableInfo);
    emit fileKeyChanged(oldKey, newKey, mutableInfo.displayName, mutableInfo.isModified);
    emit fileModifiedStateChanged(newKey, false);

    if (currentFileKey == oldKey) {
        currentFileKey = newKey;
        currentFilePath = normalizedPath;
        refreshCurrentFileTitle();
        emit currentFileChanged(newKey);
    }
    return true;
}

void EditorWidget::closeFileInternal(const QString& fileKey)
{
    if (!openedFiles.contains(fileKey)) return;
    const bool isCurrent = (fileKey == currentFileKey);
    openedFiles.remove(fileKey);
    emit fileClosed(fileKey);

    if (!isCurrent) return;
    if (openedFiles.isEmpty()) {
        currentFileKey.clear();
        currentFilePath.clear();
        isInternalContentLoading = true;
        sqlEditor->clear();
        isInternalContentLoading = false;
        refreshCurrentFileTitle();
        return;
    }
    const QString nextFileKey = openedFiles.firstKey();
    loadFileStateToEditor(nextFileKey);
    emit currentFileChanged(nextFileKey);
}
