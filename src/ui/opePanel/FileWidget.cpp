/**
 * @file FileWidget.cpp
 * @author YuzhSong
 * @brief 左侧已打开文件列表面板实现文件
 * @details 只处理文件列表展示与激活信号，不处理 EditorWidget 的读写与执行逻辑
 * @module ui/opePanel
 */

#include "FileWidget.h"

#include <QAbstractItemView>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QSizePolicy>
#include <QVBoxLayout>

FileWidget::FileWidget(QWidget* parent)
    : QWidget(parent)
    , openedFileList(nullptr)
{
    initUI();
    initStyle();
    initConnections();
}

void FileWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    auto* titleLabel = new QLabel(tr("已打开文件"), this);
    titleLabel->setObjectName("fileTitleLabel");
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    openedFileList = new QListWidget(this);
    openedFileList->setObjectName("fileOpenedFileList");
    openedFileList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    openedFileList->setSelectionMode(QAbstractItemView::SingleSelection);
    openedFileList->setAlternatingRowColors(true);
    openedFileList->setContextMenuPolicy(Qt::CustomContextMenu);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(openedFileList, 1);
    setupContextMenu();
}

void FileWidget::initConnections()
{
    // 作者：YuzhSong
    // FileWidget 只负责把“用户点中了哪个文件”通过信号抛出，不直接操控 EditorWidget。
    connect(openedFileList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (!item) {
            return;
        }
        const QString fileKey = item->data(Qt::UserRole).toString();
        if (!fileKey.isEmpty()) {
            emit fileActivated(fileKey);
        }
    });

    connect(openedFileList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (!item) {
            return;
        }
        const QString fileKey = item->data(Qt::UserRole).toString();
        if (!fileKey.isEmpty()) {
            emit fileActivated(fileKey);
        }
    });
}

void FileWidget::initStyle()
{
    setStyleSheet(
        "FileWidget {"
        "    background-color: #111315;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 14px;"
        "}"
        "QLabel#fileTitleLabel {"
        "    color: #D5D8DD;"
        "    font-weight: 700;"
        "    background-color: #2F3134;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 8px;"
        "    padding: 8px 10px;"
        "}"
        "QListWidget#fileOpenedFileList {"
        "    background-color: #111315;"
        "    color: #F0F0F0;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 10px;"
        "    padding: 6px;"
        "    outline: none;"
        "}"
        "QListWidget#fileOpenedFileList::item {"
        "    min-height: 23px;"
        "    max-height: 23px;"
        "    padding: 4px 8px;"
        "    margin: 1px 2px;"
        "    border-radius: 4px;"
        "}"
        "QListWidget#fileOpenedFileList::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QListWidget#fileOpenedFileList::item:selected {"
        "    background-color: #2F64A8;"
        "    color: #FFFFFF;"
        "}"
    );
}

void FileWidget::addOpenedFile(const QString& fileKey, const QString& displayName, bool modified)
{
    // 作者：YuzhSong
    // fileKey 是 FileWidget 与 EditorWidget 解耦的关键：FileWidget 不关心它是路径还是临时 key，只做列表展示。
    if (fileKey.isEmpty() || displayName.isEmpty()) {
        return;
    }

    QListWidgetItem* existingItem = findFileItem(fileKey);
    if (existingItem != nullptr) {
        existingItem->setText(buildDisplayText(displayName, modified));
        existingItem->setData(Qt::UserRole + 1, displayName);
        existingItem->setData(Qt::UserRole + 2, modified);
        openedFileList->setCurrentItem(existingItem);
        return;
    }

    auto* item = new QListWidgetItem(buildDisplayText(displayName, modified), openedFileList);
    item->setData(Qt::UserRole, fileKey);
    item->setData(Qt::UserRole + 1, displayName);
    item->setData(Qt::UserRole + 2, modified);
    item->setToolTip(displayName);
    openedFileList->addItem(item);
    openedFileList->setCurrentItem(item);
}

void FileWidget::setCurrentFile(const QString& fileKey)
{
    QListWidgetItem* item = findFileItem(fileKey);
    if (item != nullptr) {
        openedFileList->setCurrentItem(item);
    }
}

void FileWidget::updateFileModifiedState(const QString& fileKey, bool modified)
{
    QListWidgetItem* item = findFileItem(fileKey);
    if (item == nullptr) {
        return;
    }
    const QString displayName = item->data(Qt::UserRole + 1).toString();
    item->setData(Qt::UserRole + 2, modified);
    item->setText(buildDisplayText(displayName, modified));
}

void FileWidget::removeOpenedFile(const QString& fileKey)
{
    QListWidgetItem* item = findFileItem(fileKey);
    if (item == nullptr || !openedFileList) {
        return;
    }

    delete openedFileList->takeItem(openedFileList->row(item));
}

void FileWidget::updateFileKey(const QString& oldFileKey, const QString& newFileKey, const QString& newDisplayName, bool modified)
{
    QListWidgetItem* item = findFileItem(oldFileKey);
    if (item == nullptr || newFileKey.isEmpty() || newDisplayName.isEmpty()) {
        return;
    }

    item->setData(Qt::UserRole, newFileKey);
    item->setData(Qt::UserRole + 1, newDisplayName);
    item->setData(Qt::UserRole + 2, modified);
    item->setText(buildDisplayText(newDisplayName, modified));
    item->setToolTip(newDisplayName);
}

QListWidgetItem* FileWidget::findFileItem(const QString& fileKey) const
{
    if (!openedFileList || fileKey.isEmpty()) {
        return nullptr;
    }

    for (int i = 0; i < openedFileList->count(); ++i) {
        QListWidgetItem* item = openedFileList->item(i);
        if (!item) {
            continue;
        }

        const QString itemKey = item->data(Qt::UserRole).toString();
        if (itemKey == fileKey) {
            return item;
        }
    }

    return nullptr;
}

QString FileWidget::buildDisplayText(const QString& displayName, bool modified) const
{
    return modified ? QString("%1 *").arg(displayName) : displayName;
}

void FileWidget::setupContextMenu()
{
    // 作者：YuzhSong
    // FileWidget 右键菜单只做“发请求”，不做保存判断与文件读写，避免侵入 EditorWidget 业务状态。
    connect(openedFileList, &QListWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QListWidgetItem* item = openedFileList->itemAt(pos);
        if (item == nullptr) {
            return;
        }

        QMenu menu(this);
        QAction* closeAction = menu.addAction(tr("关闭文件"));
        QAction* selectedAction = menu.exec(openedFileList->viewport()->mapToGlobal(pos));
        if (selectedAction != closeAction) {
            return;
        }

        const QString fileKey = item->data(Qt::UserRole).toString();
        if (!fileKey.isEmpty()) {
            emit closeFileRequested(fileKey);
        }
    });
}
