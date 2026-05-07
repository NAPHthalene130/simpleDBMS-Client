/**
 * @file DirectoryWidget.cpp
 * @brief 左侧目录与已打开文件列表组件实现文件
 * @details 仅实现左侧导航 UI 与交互，不承担文件读取、保存、SQL 运行等业务逻辑。
 *          通过 fileActivated 信号将用户操作上抛到协调层（OpePanelWidget），保持解耦。
 * @author YuzhSong
 * @date 2026-04-26
 */

#include "DirectoryWidget.h"

#include <QAbstractItemView>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <QVBoxLayout>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param mainWindow 主窗口指针
 * @param parent 父组件指针
 */
DirectoryWidget::DirectoryWidget(MainWindow* mainWindow, QWidget* parent)
    : QWidget(parent)
    , mainWindow(mainWindow)
    , openedFileList(nullptr)
{
    // 允许左侧面板在 QSplitter 中被完全压缩到 0，满足 IDE 侧栏收起需求。
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    initUI();
    initStyle();
    initConnections();
}

/**
 * @brief 获取主窗口指针
 * @author YuzhSong
 * @return 主窗口指针
 */
MainWindow* DirectoryWidget::getMainWindow() const
{
    return mainWindow;
}

/**
 * @brief 初始化 UI
 * @author YuzhSong
 */
void DirectoryWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

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
    // 作者：YuzhSong
    // 目录区仅做样式增强：深灰背景、边框、hover 与选中态统一，不改变文件激活逻辑。
    // 作者：YuzhSong
    // 面板圆角统一为 14px，内部列表仅保留弱边框和 8px 圆角，避免内部再形成突兀大框。
    setStyleSheet(
        "DirectoryWidget {"
        "    background-color: #252629;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 14px;"
        "}"
        "QLabel#directoryTitleLabel {"
        "    color: #F0F0F0;"
        "    font-weight: 600;"
        "    padding-left: 2px;"
        "}"
        "QListWidget#directoryOpenedFileList {"
        "    background-color: #1F2023;"
        "    color: #F0F0F0;"
        "    border: 1px solid #2F3136;"
        "    border-radius: 8px;"
        "    outline: none;"
        "}"
        "QListWidget#directoryOpenedFileList::item {"
        "    padding: 7px 8px;"
        "    margin: 1px 2px;"
        "    border-radius: 4px;"
        "}"
        "QListWidget#directoryOpenedFileList::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QListWidget#directoryOpenedFileList::item:selected {"
        "    background-color: #3D6FB6;"
        "    color: #FFFFFF;"
        "}"
    );
}

/**
 * @brief 初始化信号槽连接
 * @author YuzhSong
 */
void DirectoryWidget::initConnections()
{
    // 目录组件只负责发出“激活了哪个文件”的事件，不直接操作 EditorWidget。
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

/**
 * @brief 添加已打开文件到列表
 * @author YuzhSong
 * @param filePath 文件完整路径
 */
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

/**
 * @brief 设置当前文件选中项
 * @author YuzhSong
 * @param filePath 文件完整路径
 */
void DirectoryWidget::setCurrentFile(const QString& filePath)
{
    QListWidgetItem* item = findFileItem(filePath);
    if (item != nullptr) {
        openedFileList->setCurrentItem(item);
    }
}

/**
 * @brief 按路径查找列表项
 * @author YuzhSong
 * @param filePath 文件完整路径
 * @return 命中返回项指针，否则返回 nullptr
 */
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

/**
 * @brief 规范化路径
 * @author YuzhSong
 * @param filePath 原始路径
 * @return 规范化后的绝对路径
 */
QString DirectoryWidget::normalizeFilePath(const QString& filePath) const
{
    if (filePath.isEmpty()) {
        return QString();
    }
    return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
}

/**
 * @brief 判断两个路径是否表示同一文件
 * @author YuzhSong
 * @param leftPath 左路径
 * @param rightPath 右路径
 * @return 相同返回 true，不同返回 false
 */
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
