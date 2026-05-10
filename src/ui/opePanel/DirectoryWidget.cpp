/**
 * @file DirectoryWidget.cpp
 * @author YuzhSong
 * @brief 左侧数据库目录树面板实现文件
 * @details 当前阶段只实现前端数据库对象树与接口预留，不实现服务端元数据获取
 * @module ui/opePanel
 */

#include "DirectoryWidget.h"

#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace {
constexpr int ItemTypeRole = Qt::UserRole + 1;
constexpr int DbNameRole = Qt::UserRole + 2;
constexpr int TableNameRole = Qt::UserRole + 3;
}

DirectoryWidget::DirectoryWidget(QWidget* parent)
    : QWidget(parent)
    , directoryTree(nullptr)
{
    initUI();
    initStyle();
    initConnections();
    loadMockDirectory();
}

void DirectoryWidget::initUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    auto* titleLabel = new QLabel(tr("数据库目录"), this);
    titleLabel->setObjectName("directoryTitleLabel");

    directoryTree = new QTreeWidget(this);
    directoryTree->setObjectName("directoryTreeWidget");
    directoryTree->setColumnCount(1);
    directoryTree->setHeaderHidden(true);
    directoryTree->setRootIsDecorated(true);
    directoryTree->setUniformRowHeights(true);
    directoryTree->setExpandsOnDoubleClick(true);
    directoryTree->header()->setStretchLastSection(true);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(directoryTree, 1);
}

void DirectoryWidget::initConnections()
{
    // 作者：YuzhSong
    // 当前只发出 table/column 激活信号做接口预留，不直接耦合 EditorWidget 业务逻辑。
    connect(directoryTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        handleItemDoubleClicked(item);
    });
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
        "QTreeWidget#directoryTreeWidget {"
        "    background-color: #111315;"
        "    color: #F0F0F0;"
        "    border: 1px solid #1B1D20;"
        "    border-radius: 10px;"
        "    padding: 6px;"
        "    outline: none;"
        "}"
        "QTreeWidget#directoryTreeWidget::item {"
        "    padding: 3px 6px;"
        "    margin: 1px 2px;"
        "    border-radius: 4px;"
        "}"
        "QTreeWidget#directoryTreeWidget::item:hover {"
        "    background-color: #34373C;"
        "}"
        "QTreeWidget#directoryTreeWidget::item:selected {"
        "    background-color: #2F64A8;"
        "    color: #FFFFFF;"
        "}"
    );
}

void DirectoryWidget::clearDirectory()
{
    directoryTree->clear();
}

void DirectoryWidget::loadMockDirectory()
{
    // 作者：YuzhSong
    // 当前阶段使用前端模拟树结构，后续由 refreshDirectory(服务端元数据) 替换。
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

void DirectoryWidget::refreshDirectory(const QStringList& databaseNames)
{
    // 作者：YuzhSong
    // TODO：后续这里会接收服务端完整元数据（数据库/表/字段/索引等）并重建树结构。
    // 当前先用简化数据库名列表占位，保证接口已预留且不影响编译。
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
    if (!item) {
        return;
    }

    const QString itemType = item->data(0, ItemTypeRole).toString();
    const QString dbName = item->data(0, DbNameRole).toString();
    const QString tableName = item->data(0, TableNameRole).toString();

    if (itemType == QStringLiteral("table")) {
        emit tableActivated(dbName, tableName);
        return;
    }

    if (itemType == QStringLiteral("column")) {
        const QString columnText = item->text(0);
        const QString columnName = columnText.section(':', 0, 0).trimmed();
        emit columnActivated(dbName, tableName, columnName);
    }
}

