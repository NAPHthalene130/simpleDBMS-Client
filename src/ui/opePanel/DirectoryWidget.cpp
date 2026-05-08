#include "DirectoryWidget.h"

#include <QAbstractItemView>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <QVBoxLayout>

DirectoryWidget::DirectoryWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , openedFileList(nullptr)
{
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    initUI();
    initStyle();
    initConnections();
}

MainWindow* DirectoryWidget::getMainWindow() const
{
    return mainWindow;
}

void DirectoryWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    auto* titleLabel = new QLabel(tr("已打开文件"), this);
    titleLabel->setObjectName("directoryTitleLabel");
    titleLabel->setMinimumWidth(0);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    openedFileList = new QListWidget(this);
    openedFileList->setObjectName("directoryOpenedFileList");
    openedFileList->setMinimumWidth(0);
    openedFileList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    openedFileList->setSelectionMode(QAbstractItemView::SingleSelection);
    openedFileList->setAlternatingRowColors(true);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(openedFileList, 1);
}

void DirectoryWidget::initStyle()
{
    setStyleSheet(
        "DirectoryWidget {"
        "    background-color: #111315;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 14px;"
        "}"
        "QLabel#directoryTitleLabel {"
        "    color: #D5D8DD;"
        "    font-weight: 700;"
        "    background-color: #2F3134;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 8px;"
        "    padding: 8px 10px;"
        "}"
        "QListWidget#directoryOpenedFileList {"
        "    background-color: #111315;"
        "    color: #F0F0F0;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 10px;"
        "    padding: 6px;"
        "    outline: none;"
        "}"
        "QListWidget#directoryOpenedFileList::item {"
        "    min-height: 23px;"
        "    max-height: 23px;"
        "    padding: 4px 8px;"
        "    margin: 1px 2px;"
        "    border-radius: 4px;"
        "}"
        "QListWidget#directoryOpenedFileList::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QListWidget#directoryOpenedFileList::item:selected {"
        "    background-color: #2F64A8;"
        "    color: #FFFFFF;"
        "}"
    );
}

void DirectoryWidget::initConnections()
{
    connect(openedFileList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (!item) {
            return;
        }

        const QString filePath = item->data(Qt::UserRole).toString();
        if (!filePath.isEmpty()) {
            emit fileActivated(filePath);
        }
    });

    connect(openedFileList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (!item) {
            return;
        }

        const QString filePath = item->data(Qt::UserRole).toString();
        if (!filePath.isEmpty()) {
            emit fileActivated(filePath);
        }
    });
}

void DirectoryWidget::addOpenedFile(const QString& filePath)
{
    const QString normalizedFilePath = normalizeFilePath(filePath);
    if (normalizedFilePath.isEmpty()) {
        return;
    }

    QListWidgetItem* existingItem = findFileItem(normalizedFilePath);
    if (existingItem != nullptr) {
        openedFileList->setCurrentItem(existingItem);
        return;
    }

    const QFileInfo fileInfo(normalizedFilePath);
    auto* item = new QListWidgetItem(fileInfo.fileName(), openedFileList);
    item->setData(Qt::UserRole, normalizedFilePath);
    item->setToolTip(normalizedFilePath);
    openedFileList->addItem(item);
    openedFileList->setCurrentItem(item);
}

void DirectoryWidget::setCurrentFile(const QString& filePath)
{
    QListWidgetItem* item = findFileItem(filePath);
    if (item != nullptr) {
        openedFileList->setCurrentItem(item);
    }
}

QListWidgetItem* DirectoryWidget::findFileItem(const QString& filePath) const
{
    if (!openedFileList) {
        return nullptr;
    }

    const QString normalizedFilePath = normalizeFilePath(filePath);
    for (int i = 0; i < openedFileList->count(); ++i) {
        QListWidgetItem* item = openedFileList->item(i);
        if (!item) {
            continue;
        }

        const QString itemPath = item->data(Qt::UserRole).toString();
        if (isSameFilePath(itemPath, normalizedFilePath)) {
            return item;
        }
    }

    return nullptr;
}

QString DirectoryWidget::normalizeFilePath(const QString& filePath) const
{
    if (filePath.isEmpty()) {
        return QString();
    }
    return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
}

bool DirectoryWidget::isSameFilePath(const QString& leftPath, const QString& rightPath) const
{
    const QString normalizedLeft = normalizeFilePath(leftPath);
    const QString normalizedRight = normalizeFilePath(rightPath);

#ifdef Q_OS_WIN
    return QString::compare(normalizedLeft, normalizedRight, Qt::CaseInsensitive) == 0;
#else
    return normalizedLeft == normalizedRight;
#endif
}
