/**
 * @file DirectoryWidget.cpp
 * @author YuzhSong
 * @brief 左侧数据库目录树面板实现文件
 * @details 当前阶段仅实现前端目录树展示与刷新接口预留，不实现真实服务端通信
 * @module ui/opePanel
 */

#include "DirectoryWidget.h"
#include "ui/ThemeManager.h"

#include <QDebug>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "models/network/NetworkTransferData.h"

namespace {
constexpr int ItemTypeRole = Qt::UserRole + 1;
constexpr int DbNameRole = Qt::UserRole + 2;
constexpr int TableNameRole = Qt::UserRole + 3;
}

DirectoryWidget::DirectoryWidget(QWidget* parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , refreshButton(nullptr)
    , directoryTree(nullptr)
{
    initUI();
    initStyle();
    initConnections();
}

void DirectoryWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // 作者：YuzhSong
    // 顶部区域采用水平布局：左侧标题，右侧 Refresh 按钮。
    // 该布局仅作用于 DirectoryWidget 内部，不修改其他面板布局。
    auto* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(8);

    titleLabel = new QLabel(tr("数据库目录"), this);
    titleLabel->setObjectName("directoryTitleLabel");

    refreshButton = new QPushButton(tr("Refresh"), this);
    refreshButton->setObjectName("directoryRefreshButton");
    refreshButton->setCursor(Qt::PointingHandCursor);
    refreshButton->setToolTip(tr("刷新数据库目录"));

    topLayout->addWidget(titleLabel, 0);
    topLayout->addStretch(1);
    topLayout->addWidget(refreshButton, 0);

    directoryTree = new QTreeWidget(this);
    directoryTree->setObjectName("directoryTreeWidget");
    directoryTree->setColumnCount(1);
    directoryTree->setHeaderHidden(true);
    directoryTree->setRootIsDecorated(true);
    directoryTree->setUniformRowHeights(true);
    directoryTree->setExpandsOnDoubleClick(true);
    // 作者：YuzhSong
    // 保留原有目录树层级缩进策略，避免深层节点文本被挤出可视区域。
    directoryTree->setIndentation(12);
    directoryTree->header()->setStretchLastSection(true);
    directoryTree->setContextMenuPolicy(Qt::CustomContextMenu);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(directoryTree, 1);
}

void DirectoryWidget::initConnections()
{
    connect(directoryTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        handleItemDoubleClicked(item);
    });

    connect(directoryTree, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QTreeWidgetItem *item = directoryTree->itemAt(pos);
        if (item == nullptr) {
            return;
        }
        const QString itemType = item->data(0, ItemTypeRole).toString();
        const QString dbName = item->data(0, DbNameRole).toString();
        const QString tableName = item->data(0, TableNameRole).toString();

        if (itemType == QStringLiteral("table")) {
            emit tableActivated(dbName, tableName);
        } else if (itemType == QStringLiteral("column")) {
            const QString columnName = item->text(0).section(':', 0, 0).trimmed();
            emit columnActivated(dbName, tableName, columnName);
        } else if (itemType == QStringLiteral("database")) {
            emit databaseActivated(dbName);
        }
    });

    connect(refreshButton, &QPushButton::clicked, this, &DirectoryWidget::requestRefreshDirectory);
}

void DirectoryWidget::initStyle()
{
    setStyleSheet(ThemeManager::sidePanel());
}

void DirectoryWidget::clearDirectory()
{
    directoryTree->clear();
}

void DirectoryWidget::loadMockDirectory()
{
    // 作者：YuzhSong
    // 当前阶段使用前端模拟目录树结构，后续由 refreshDirectory(服务端元数据) 替换。
    clearDirectory();

    auto* rootNode = new QTreeWidgetItem(directoryTree);
    rootNode->setText(0, tr("simpleDBMS"));
    rootNode->setData(0, ItemTypeRole, QStringLiteral("root"));

    auto* databasesNode = new QTreeWidgetItem(rootNode);
    databasesNode->setText(0, tr("databases"));
    databasesNode->setData(0, ItemTypeRole, QStringLiteral("databases"));

    QTreeWidgetItem* schoolNode = createDatabaseNode(databasesNode, QStringLiteral("school"));
    QTreeWidgetItem* schoolTablesNode = schoolNode->child(0);
    createTableNode(
        schoolTablesNode,
        QStringLiteral("school"),
        QStringLiteral("student"),
        {QStringLiteral("id : INT"), QStringLiteral("name : VARCHAR"), QStringLiteral("age : INT")}
    );
    createTableNode(
        schoolTablesNode,
        QStringLiteral("school"),
        QStringLiteral("course"),
        {QStringLiteral("id : INT"), QStringLiteral("title : VARCHAR"), QStringLiteral("credit : INT")}
    );

    QTreeWidgetItem* testDbNode = createDatabaseNode(databasesNode, QStringLiteral("test_db"));
    QTreeWidgetItem* testTablesNode = testDbNode->child(0);
    createTableNode(testTablesNode, QStringLiteral("test_db"), QStringLiteral("demo_table"), {QStringLiteral("id : INT")});

    directoryTree->expandItem(rootNode);
    directoryTree->expandItem(databasesNode);
    directoryTree->expandItem(schoolNode);
}

void DirectoryWidget::requestRefreshDirectory()
{
    /**
     * @brief 响应 Refresh 按钮点击，请求重新获取服务端目录结构
     * @details 仅发送信号通知外部模块发起网络请求，
     *          目录信息由服务端 DIRECTORY_RESPONSE 回到 NetReceiver 后刷新。
     * @author NAPH130
     */
    emit refreshRequested();
}

void DirectoryWidget::refreshDirectory(const QStringList& databaseNames)
{
    // 作者：YuzhSong
    // TODO: 后续这里会接收服务端完整元数据（数据库/表/字段/索引等）并重建树结构。
    // 作者：YuzhSong，当前仅预留简单版本（QStringList），后续应替换为数据库元数据模型结构。
    clearDirectory();

    auto* rootNode = new QTreeWidgetItem(directoryTree);
    rootNode->setText(0, tr("simpleDBMS"));
    rootNode->setData(0, ItemTypeRole, QStringLiteral("root"));

    auto* databasesNode = new QTreeWidgetItem(rootNode);
    databasesNode->setText(0, tr("databases"));
    databasesNode->setData(0, ItemTypeRole, QStringLiteral("databases"));

    for (const QString& dbName : databaseNames) {
        createDatabaseNode(databasesNode, dbName);
    }

    directoryTree->expandItem(rootNode);
    directoryTree->expandItem(databasesNode);
}

/**
 * @brief 根据服务端返回的完整目录结构刷新目录树
 * @details
 * 1. 清空现有树结构后重建根节点与 databases 分组节点。
 * 2. 遍历 DatabaseNode 列表，为每个数据库创建节点并填充其下属 TableNode。
 * 3. 每个 TableNode 携带的 fields 列表直接作为列节点展示。
 * @param databases 服务端 DIRECTORY_RESPONSE 中的数据库目录列表
 * @author NAPH130
 */
void DirectoryWidget::refreshFromServer(const std::vector<DatabaseNode>& databases)
{
    clearDirectory();
    dbVersionMap.clear();

    auto* rootNode = new QTreeWidgetItem(directoryTree);
    rootNode->setText(0, tr("simpleDBMS"));
    rootNode->setData(0, ItemTypeRole, QStringLiteral("root"));

    auto* databasesNode = new QTreeWidgetItem(rootNode);
    databasesNode->setText(0, tr("databases"));
    databasesNode->setData(0, ItemTypeRole, QStringLiteral("databases"));

    for (const DatabaseNode& databaseNode : databases) {
        const QString dbName = QString::fromStdString(databaseNode.getName());
        dbVersionMap[dbName] = databaseNode.getDbVersion();

        QTreeWidgetItem* dbNode = createDatabaseNode(databasesNode, dbName);
        QTreeWidgetItem* tablesNode = dbNode->child(0);

        for (const TableNode& tableNode : databaseNode.getTables()) {
            const QString tableName = QString::fromStdString(tableNode.getName());
            QStringList columnTexts;
            const auto& fields = tableNode.getFields();
            columnTexts.reserve(static_cast<qsizetype>(fields.size()));
            for (const std::string& field : fields) {
                columnTexts.append(QString::fromStdString(field));
            }
            createTableNode(tablesNode, dbName, tableName, columnTexts);
        }
    }

    directoryTree->expandItem(rootNode);
    directoryTree->expandItem(databasesNode);
}

std::uint64_t DirectoryWidget::getDbVersion(const QString& dbName) const
{
    return dbVersionMap.value(dbName, 0);
}

void DirectoryWidget::setDbVersion(const QString& dbName, std::uint64_t version)
{
    dbVersionMap[dbName] = version;
}

QTreeWidgetItem* DirectoryWidget::createDatabaseNode(QTreeWidgetItem* root, const QString& databaseName)
{
    auto* dbNode = new QTreeWidgetItem(root);
    dbNode->setText(0, databaseName);
    dbNode->setData(0, ItemTypeRole, QStringLiteral("database"));
    dbNode->setData(0, DbNameRole, databaseName);

    auto* tablesNode = new QTreeWidgetItem(dbNode);
    tablesNode->setText(0, tr("Tables"));
    tablesNode->setData(0, ItemTypeRole, QStringLiteral("tables"));
    tablesNode->setData(0, DbNameRole, databaseName);

    auto* viewsNode = new QTreeWidgetItem(dbNode);
    viewsNode->setText(0, tr("Views"));
    viewsNode->setData(0, ItemTypeRole, QStringLiteral("views"));
    viewsNode->setData(0, DbNameRole, databaseName);

    auto* proceduresNode = new QTreeWidgetItem(dbNode);
    proceduresNode->setText(0, tr("Procedures"));
    proceduresNode->setData(0, ItemTypeRole, QStringLiteral("procedures"));
    proceduresNode->setData(0, DbNameRole, databaseName);

    auto* functionsNode = new QTreeWidgetItem(dbNode);
    functionsNode->setText(0, tr("Functions"));
    functionsNode->setData(0, ItemTypeRole, QStringLiteral("functions"));
    functionsNode->setData(0, DbNameRole, databaseName);

    return dbNode;
}

void DirectoryWidget::createTableNode(QTreeWidgetItem* tablesNode,
                                      const QString& databaseName,
                                      const QString& tableName,
                                      const QStringList& columns)
{
    auto* tableNode = new QTreeWidgetItem(tablesNode);
    tableNode->setText(0, tableName);
    tableNode->setData(0, ItemTypeRole, QStringLiteral("table"));
    tableNode->setData(0, DbNameRole, databaseName);
    tableNode->setData(0, TableNameRole, tableName);

    auto* columnsNode = new QTreeWidgetItem(tableNode);
    columnsNode->setText(0, tr("Columns"));
    columnsNode->setData(0, ItemTypeRole, QStringLiteral("columns"));
    columnsNode->setData(0, DbNameRole, databaseName);
    columnsNode->setData(0, TableNameRole, tableName);

    for (const QString& columnText : columns) {
        auto* columnNode = new QTreeWidgetItem(columnsNode);
        columnNode->setText(0, columnText);
        columnNode->setData(0, ItemTypeRole, QStringLiteral("column"));
        columnNode->setData(0, DbNameRole, databaseName);
        columnNode->setData(0, TableNameRole, tableName);
    }

    auto* indexesNode = new QTreeWidgetItem(tableNode);
    indexesNode->setText(0, tr("Indexes"));
    indexesNode->setData(0, ItemTypeRole, QStringLiteral("indexes"));
    indexesNode->setData(0, DbNameRole, databaseName);
    indexesNode->setData(0, TableNameRole, tableName);
}

void DirectoryWidget::handleItemDoubleClicked(QTreeWidgetItem* item)
{
    Q_UNUSED(item);
}

void DirectoryWidget::refreshTheme()
{
    setStyleSheet(ThemeManager::sidePanel());
}
