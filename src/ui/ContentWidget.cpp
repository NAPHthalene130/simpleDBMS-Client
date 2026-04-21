#include "contentwidget.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QTimer>

// ---------------------- DatabaseTreeWidget ----------------------
DatabaseTreeWidget::DatabaseTreeWidget(QWidget *parent)
    : QTreeWidget(parent), m_contextMenu(new QMenu(this))
{
    setHeaderLabel("数据库结构 (模拟)");
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::itemExpanded, this, &DatabaseTreeWidget::onItemExpanded);
    connect(this, &QTreeWidget::customContextMenuRequested,
            this, &DatabaseTreeWidget::onCustomContextMenu);

    // 构建右键菜单（预留接口）
    QAction *refreshAct = m_contextMenu->addAction("刷新");
    connect(refreshAct, &QAction::triggered, this, &DatabaseTreeWidget::onRefresh);
    m_contextMenu->addSeparator();
    QAction *openTableAct = m_contextMenu->addAction("打开表");
    connect(openTableAct, &QAction::triggered, this, &DatabaseTreeWidget::onOpenTable);
    // 以下菜单项预留，暂时禁用
    QAction *designAct = m_contextMenu->addAction("设计表");
    designAct->setEnabled(false);
    QAction *dropAct = m_contextMenu->addAction("删除表");
    dropAct->setEnabled(false);
}

void DatabaseTreeWidget::setConnectionInfo(const QString &host, int port, const QString &user)
{
    m_host = host;
    m_port = port;
    m_user = user;
}

void DatabaseTreeWidget::loadDatabases()
{
    clear();

    // 创建服务器根节点
    QTreeWidgetItem *serverItem = new QTreeWidgetItem(this);
    serverItem->setText(0, QString("%1@%2:%3 (模拟)")
                        .arg(m_user.isEmpty() ? "root" : m_user)
                        .arg(m_host.isEmpty() ? "localhost" : m_host)
                        .arg(m_port == 0 ? 3306 : m_port));
    serverItem->setIcon(0, style()->standardIcon(QStyle::SP_DriveNetIcon));
    serverItem->setData(0, Qt::UserRole + 1, TypeServer);
    addTopLevelItem(serverItem);

    // 模拟数据库列表
    QStringList mockDatabases = {"test_db", "company", "school", "blog"};
    for (const QString &dbName : mockDatabases) {
        QTreeWidgetItem *dbItem = new QTreeWidgetItem(serverItem);
        dbItem->setText(0, dbName);
        dbItem->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        dbItem->setData(0, Qt::UserRole + 1, TypeDatabase);
        dbItem->setData(0, Qt::UserRole, dbName);
        // 添加占位节点，使其可展开
        new QTreeWidgetItem(dbItem);
    }

    serverItem->setExpanded(true);
}

void DatabaseTreeWidget::onItemExpanded(QTreeWidgetItem *item)
{
    if (!item || item->childCount() != 1 || item->child(0)->text(0).isEmpty())
        return;

    int type = item->data(0, Qt::UserRole + 1).toInt();
    if (type == TypeDatabase)
        loadTables(item);
    else if (type == TypeTable)
        loadColumns(item);
}

void DatabaseTreeWidget::loadTables(QTreeWidgetItem *dbItem)
{
    // 清除占位节点
    qDeleteAll(dbItem->takeChildren());

    QString dbName = dbItem->data(0, Qt::UserRole).toString();

    // 模拟每个数据库下的表
    QMap<QString, QStringList> mockTables;
    mockTables["test_db"] = {"users", "products", "orders"};
    mockTables["company"] = {"employees", "departments", "salaries"};
    mockTables["school"] = {"students", "courses", "enrollments"};
    mockTables["blog"] = {"posts", "comments", "tags"};

    QStringList tables = mockTables.value(dbName, {"table1", "table2"});
    for (const QString &tableName : tables) {
        QTreeWidgetItem *tableItem = new QTreeWidgetItem(dbItem);
        tableItem->setText(0, tableName);
        tableItem->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
        tableItem->setData(0, Qt::UserRole + 1, TypeTable);
        tableItem->setData(0, Qt::UserRole, dbName);
        tableItem->setData(1, Qt::UserRole, tableName);
        // 添加占位节点
        new QTreeWidgetItem(tableItem);
    }
}

void DatabaseTreeWidget::loadColumns(QTreeWidgetItem *tableItem)
{
    qDeleteAll(tableItem->takeChildren());

    QString dbName = tableItem->data(0, Qt::UserRole).toString();
    QString tableName = tableItem->data(1, Qt::UserRole).toString();

    // 模拟字段定义
    QList<QPair<QString, QString>> mockColumns;
    if (tableName == "users") {
        mockColumns = {{"id", "INT"}, {"username", "VARCHAR(50)"}, {"email", "VARCHAR(100)"}};
    } else if (tableName == "products") {
        mockColumns = {{"id", "INT"}, {"name", "VARCHAR(100)"}, {"price", "DECIMAL(10,2)"}};
    } else {
        mockColumns = {{"id", "INT"}, {"name", "VARCHAR(255)"}, {"created_at", "DATETIME"}};
    }

    for (const auto &col : mockColumns) {
        QString colInfo = QString("%1 (%2)").arg(col.first, col.second);
        if (col.first == "id")
            colInfo += " [主键]";

        QTreeWidgetItem *colItem = new QTreeWidgetItem(tableItem);
        colItem->setText(0, colInfo);
        colItem->setIcon(0, style()->standardIcon(QStyle::SP_FileLinkIcon));
        colItem->setData(0, Qt::UserRole + 1, TypeColumn);
    }
}

void DatabaseTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QTreeWidgetItem *item = itemAt(event->pos());
    if (item)
        m_contextMenu->exec(event->globalPos());
}

void DatabaseTreeWidget::onCustomContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = itemAt(pos);
    if (item)
        m_contextMenu->exec(mapToGlobal(pos));
}

void DatabaseTreeWidget::onRefresh()
{
    QTreeWidgetItem *item = currentItem();
    if (!item) return;

    int type = item->data(0, Qt::UserRole + 1).toInt();
    if (type == TypeDatabase)
        loadTables(item);
    else if (type == TypeTable)
        loadColumns(item);
}

void DatabaseTreeWidget::onOpenTable()
{
    QTreeWidgetItem *item = currentItem();
    if (item && item->data(0, Qt::UserRole + 1).toInt() == TypeTable) {
        emit openTableRequested(databaseFromItem(item), tableFromItem(item));
    }
}

QString DatabaseTreeWidget::databaseFromItem(QTreeWidgetItem *item) const
{
    int type = item->data(0, Qt::UserRole + 1).toInt();
    if (type == TypeDatabase)
        return item->data(0, Qt::UserRole).toString();
    if (type == TypeTable || type == TypeColumn)
        return item->data(0, Qt::UserRole).toString();
    return QString();
}

QString DatabaseTreeWidget::tableFromItem(QTreeWidgetItem *item) const
{
    int type = item->data(0, Qt::UserRole + 1).toInt();
    if (type == TypeTable)
        return item->data(1, Qt::UserRole).toString();
    if (type == TypeColumn)
        return item->data(1, Qt::UserRole).toString();
    return QString();
}

void DatabaseTreeWidget::refreshCurrentNode()
{
    onRefresh();
}

// ---------------------- ContentWidget ----------------------
ContentWidget::ContentWidget(const QString &host, int port,
                             const QString &user, const QString &password,
                             QWidget *parent)
    : QWidget(parent), m_host(host), m_port(port), m_user(user)
{
    Q_UNUSED(password)
    setupUI();

    connect(m_tree, &DatabaseTreeWidget::openTableRequested,
            this, &ContentWidget::onOpenTable);
}

ContentWidget::~ContentWidget() {}

void ContentWidget::setupUI()
{
    m_tree = new DatabaseTreeWidget(this);
    m_tree->setConnectionInfo(m_host, m_port, m_user);
    m_tree->loadDatabases();

    m_queryEditor = new QTextEdit(this);
    m_resultView = new QTextEdit(this);
    m_resultView->setReadOnly(true);

    // 默认显示一条提示
    m_queryEditor->setPlaceholderText("在此输入 SQL 语句 (当前为模拟模式，不会真正执行)");

    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(m_queryEditor);
    rightSplitter->addWidget(m_resultView);
    rightSplitter->setSizes({250, 150});

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(m_tree);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setSizes({250, 750});

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mainSplitter);
}

void ContentWidget::onExecuteQuery()
{
    QString sql = m_queryEditor->toPlainText().trimmed();
    if (sql.isEmpty()) {
        m_resultView->setText("请输入 SQL 语句。");
        return;
    }

    // 模拟执行结果
    QString result = "模拟执行成功！\n";
    result += "SQL: " + sql + "\n\n";
    result += "id\tname\t\tvalue\n";
    result += "----------------------------------\n";
    result += "1\t测试数据1\t100\n";
    result += "2\t测试数据2\t200\n";
    result += "3\t测试数据3\t300\n";
    result += "\n共 3 行 (模拟数据)";
    m_resultView->setText(result);
}

void ContentWidget::onOpenTable(const QString &database, const QString &table)
{
    m_queryEditor->setText(QString("SELECT * FROM `%1`.`%2` LIMIT 100;")
                           .arg(database, table));
    onExecuteQuery();
}