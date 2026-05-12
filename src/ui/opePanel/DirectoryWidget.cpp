/**
 * @file DirectoryWidget.cpp
 * @author YuzhSong
 * @brief 左侧数据库目录树面板实现文件
 * @details 当前阶段仅实现前端目录树展示与刷新接口预留，不实现真实服务端通信
 * @module ui/opePanel
 */

#include "DirectoryWidget.h"

#include <QDebug>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
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
    , titleLabel(nullptr)
    , refreshButton(nullptr)
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

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(directoryTree, 1);
}

void DirectoryWidget::initConnections()
{
    // 作者：YuzhSong
    // 目录树节点双击只负责发出 table/column 激活信号，不直接耦合 SQL 编辑器逻辑。
    connect(directoryTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        handleItemDoubleClicked(item);
    });

    // 作者：YuzhSong
    // Refresh 按钮点击后进入 requestRefreshDirectory()。
    // 该入口仅发信号并执行本地占位刷新，不直接发起网络请求，方便后续独立接入服务端模块。
    connect(refreshButton, &QPushButton::clicked, this, &DirectoryWidget::requestRefreshDirectory);
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
        "QPushButton#directoryRefreshButton {"
        "    color: #D5D8DD;"
        "    background-color: #2E3136;"
        "    border: 1px solid #3A3D42;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-weight: 600;"
        "}"
        "QPushButton#directoryRefreshButton:hover {"
        "    background-color: #383C42;"
        "}"
        "QPushButton#directoryRefreshButton:pressed {"
        "    background-color: #2A2D31;"
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
    // 作者：YuzhSong
    // 先发出 refreshRequested 信号，让外部模块（例如 OpePanelWidget/网络层）感知“用户请求刷新”。
    // 这里不直接请求服务端，避免把网络职责塞入目录控件内部。
    emit refreshRequested();

    // 作者：YuzhSong
    // 当前阶段仅做占位行为：日志 + 模拟刷新，便于前端联调验证按钮交互。
    qDebug() << "请求刷新数据库目录";

    // 作者：YuzhSong
    // TODO: 后续应由服务端返回真实数据库元数据后调用 refreshDirectory(...)。
    //       当前为了保持演示效果，暂时沿用 loadMockDirectory()。
    loadMockDirectory();
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
