#include "DirectoryWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QAction>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace {
constexpr int NODE_TYPE_ROLE = Qt::UserRole;
constexpr int NODE_PATH_ROLE = Qt::UserRole + 1;
}

DirectoryWidget::DirectoryWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      mainWindow(mainWindow),
      pathLabel(nullptr),
      searchEdit(nullptr),
      directoryTree(nullptr)
{
    auto *layout = new QVBoxLayout(this);
    auto *titleLabel = new QLabel(tr("目录导航"), this);
    auto *hintLabel = new QLabel(tr("进入数据库后，可在这里逐层查看连接、数据库、数据表与视图。"), this);
    auto *overviewFrame = new QFrame(this);
    auto *overviewLayout = new QHBoxLayout(overviewFrame);
    auto *databaseCountLabel = new QLabel(tr("2 个数据库"), overviewFrame);
    auto *tableCountLabel = new QLabel(tr("3 个架构 / 12 张数据表"), overviewFrame);
    auto *moodBadgeLabel = new QLabel(tr("今日贴纸：云朵模式"), overviewFrame);
    auto *toolbarLayout = new QHBoxLayout();
    searchEdit = new QLineEdit(this);
    auto *refreshButton = new QPushButton(tr("刷新"), this);
    auto *newQueryButton = new QPushButton(tr("新建查询"), this);
    pathLabel = new QLabel(tr("当前位置：本地连接 / simple_db / 架构 / public / 数据表"), this);
    auto *treeFrame = new QFrame(this);
    auto *treeLayout = new QVBoxLayout(treeFrame);
    auto *treeHeaderFrame = new QFrame(treeFrame);
    auto *treeHeaderLayout = new QHBoxLayout(treeHeaderFrame);
    auto *treeHeaderTitleLabel = new QLabel(tr("数据库资源管理器"), treeHeaderFrame);
    auto *treeHeaderHintLabel = new QLabel(tr("参考 DataGrip 的层级浏览方式"), treeHeaderFrame);
    directoryTree = new QTreeWidget(treeFrame);

    setObjectName("directoryWidget");
    overviewFrame->setObjectName("directoryOverviewFrame");
    titleLabel->setObjectName("directoryTitleLabel");
    hintLabel->setObjectName("directoryHintLabel");
    databaseCountLabel->setObjectName("directoryStatLabel");
    tableCountLabel->setObjectName("directoryStatLabel");
    moodBadgeLabel->setObjectName("directoryMoodBadgeLabel");
    refreshButton->setObjectName("directoryGhostButton");
    newQueryButton->setObjectName("directoryPrimaryButton");
    pathLabel->setObjectName("directoryPathLabel");
    treeFrame->setObjectName("directoryCard");
    treeHeaderFrame->setObjectName("directoryTreeHeaderFrame");
    treeHeaderTitleLabel->setObjectName("directoryTreeHeaderTitleLabel");
    treeHeaderHintLabel->setObjectName("directoryTreeHeaderHintLabel");

    searchEdit->setPlaceholderText(tr("搜索数据库对象，如 user、log、view"));
    directoryTree->setHeaderHidden(true);
    directoryTree->setIndentation(18);
    directoryTree->setAnimated(true);
    directoryTree->setExpandsOnDoubleClick(true);
    directoryTree->setContextMenuPolicy(Qt::CustomContextMenu);

    overviewLayout->setContentsMargins(12, 10, 12, 10);
    overviewLayout->setSpacing(8);
    overviewLayout->addWidget(databaseCountLabel);
    overviewLayout->addWidget(tableCountLabel);
    overviewLayout->addWidget(moodBadgeLabel);
    overviewLayout->addStretch();

    toolbarLayout->setSpacing(8);
    toolbarLayout->addWidget(searchEdit, 1);
    toolbarLayout->addWidget(refreshButton);
    toolbarLayout->addWidget(newQueryButton);

    treeHeaderLayout->setContentsMargins(10, 8, 10, 8);
    treeHeaderLayout->addWidget(treeHeaderTitleLabel);
    treeHeaderLayout->addStretch();
    treeHeaderLayout->addWidget(treeHeaderHintLabel);

    treeLayout->setContentsMargins(10, 10, 10, 10);
    treeLayout->setSpacing(8);
    treeLayout->addWidget(treeHeaderFrame);
    treeLayout->addWidget(directoryTree);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(overviewFrame);
    layout->addLayout(toolbarLayout);
    layout->addWidget(pathLabel);
    layout->addWidget(treeFrame, 1);

    setMinimumWidth(250);
    setMaximumWidth(360);

    setStyleSheet(
        "#directoryTitleLabel {"
        "    color: #1f2937;"
        "    font-size: 18px;"
        "    font-weight: 700;"
        "}"
        "#directoryHintLabel {"
        "    color: #667085;"
        "    font-size: 12px;"
        "}"
        "#directoryWidget {"
        "    background-color: transparent;"
        "    background-image:"
        "        radial-gradient(circle at 12% 8%, rgba(255, 214, 224, 0.85) 0, rgba(255, 214, 224, 0.85) 10px, transparent 11px),"
        "        radial-gradient(circle at 88% 14%, rgba(206, 234, 255, 0.9) 0, rgba(206, 234, 255, 0.9) 12px, transparent 13px),"
        "        radial-gradient(circle at 80% 78%, rgba(227, 215, 255, 0.85) 0, rgba(227, 215, 255, 0.85) 11px, transparent 12px),"
        "        radial-gradient(circle at 16% 86%, rgba(220, 245, 228, 0.9) 0, rgba(220, 245, 228, 0.9) 9px, transparent 10px);"
        "}"
        "#directoryOverviewFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fff7fb, stop:0.5 #f8fbff, stop:1 #f4f8ff);"
        "    border: 1px solid #e7ddf6;"
        "    border-radius: 14px;"
        "}"
        "#directoryStatLabel {"
        "    padding: 4px 10px;"
        "    border-radius: 10px;"
        "    background-color: white;"
        "    border: 1px solid #dde6f6;"
        "    color: #3550d8;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "}"
        "#directoryMoodBadgeLabel {"
        "    padding: 4px 10px;"
        "    border-radius: 10px;"
        "    background-color: #fff2fa;"
        "    border: 1px solid #f4d6ea;"
        "    color: #c05a92;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "}"
        "#directoryPathLabel {"
        "    color: #667085;"
        "    font-size: 12px;"
        "    padding-left: 2px;"
        "}"
        "#directoryTreeHeaderFrame {"
        "    background-color: #faf5ff;"
        "    border: 1px solid #eadcff;"
        "    border-radius: 12px;"
        "}"
        "#directoryTreeHeaderTitleLabel {"
        "    color: #7c3aed;"
        "    font-size: 13px;"
        "    font-weight: 700;"
        "}"
        "#directoryTreeHeaderHintLabel {"
        "    color: #9a87b8;"
        "    font-size: 11px;"
        "}"
        "#directoryCard {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ffffff, stop:1 #fbfcff);"
        "    border: 1px solid #e5def5;"
        "    border-radius: 16px;"
        "}"
        "QLineEdit {"
        "    min-height: 38px;"
        "    padding: 0 12px;"
        "    border: 1px solid #ddd7f0;"
        "    border-radius: 12px;"
        "    background-color: rgba(255, 255, 255, 0.96);"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #7c96ff;"
        "}"
        "#directoryPrimaryButton {"
        "    min-height: 38px;"
        "    padding: 0 14px;"
        "    border: none;"
        "    border-radius: 12px;"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7b8cff, stop:1 #5d6bff);"
        "    color: white;"
        "    font-weight: 700;"
        "}"
        "#directoryGhostButton {"
        "    min-height: 38px;"
        "    padding: 0 14px;"
        "    border-radius: 12px;"
        "    border: 1px solid #ddd7f0;"
        "    background-color: #fffaff;"
        "    color: #344054;"
        "    font-weight: 600;"
        "}"
        "QTreeWidget {"
        "    border: none;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 248, 252, 0.75), stop:1 rgba(245, 249, 255, 0.35));"
        "    color: #344054;"
        "    outline: none;"
        "    padding: 8px;"
        "    border-radius: 14px;"
        "}"
        "QTreeWidget::item {"
        "    min-height: 30px;"
        "    padding: 5px 8px;"
        "    border-radius: 8px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgba(255, 240, 248, 0.9);"
        "    color: #a84f86;"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: #eef3ff;"
        "    color: #3550d8;"
        "}"
        "QTreeView::branch:has-children:closed:has-siblings,"
        "QTreeView::branch:closed:has-children:has-siblings {"
        "    image: none;"
        "}"
        "QTreeView::branch:open:has-children:has-siblings,"
        "QTreeView::branch:open:has-children:!has-siblings {"
        "    image: none;"
        "}"
    );

    initializeDirectoryTree();

    connect(directoryTree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item) {
        updateCurrentPathLabel(item);
    });

    connect(directoryTree, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint &position) {
        showContextMenu(position);
    });

    connect(searchEdit, &QLineEdit::textChanged, this, [this](const QString &keyword) {
        filterDirectoryTree(keyword);
    });

    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        initializeDirectoryTree();
    });

    connect(newQueryButton, &QPushButton::clicked, this, [this]() {
        pathLabel->setText(tr("当前位置：新建查询"));
    });
}

MainWindow *DirectoryWidget::getMainWindow() const
{
    return mainWindow;
}

void DirectoryWidget::initializeDirectoryTree()
{
    const QSignalBlocker signalBlocker(directoryTree);
    directoryTree->clear();

    auto *connectionItem = createTreeItem(nullptr,
                                          tr("本地连接"),
                                          "connection",
                                          tr("本地连接"));
    auto *simpleDatabaseItem = createTreeItem(connectionItem,
                                              tr("simple_db"),
                                              "database",
                                              tr("本地连接/simple_db"));
    auto *analyticsDatabaseItem = createTreeItem(connectionItem,
                                                 tr("analytics_lab"),
                                                 "database",
                                                 tr("本地连接/analytics_lab"));
    auto *favoriteItem = createTreeItem(nullptr,
                                        tr("常用入口"),
                                        "favorite_group",
                                        tr("常用入口"));
    auto *recentItem = createTreeItem(nullptr,
                                      tr("最近访问"),
                                      "recent_group",
                                      tr("最近访问"));

    auto *schemaGroupItem = createTreeItem(simpleDatabaseItem,
                                           tr("架构"),
                                           "schema_group",
                                           tr("本地连接/simple_db/架构"));
    auto *publicSchemaItem = createTreeItem(schemaGroupItem,
                                            tr("public"),
                                            "schema",
                                            tr("本地连接/simple_db/架构/public"));
    auto *auditSchemaItem = createTreeItem(schemaGroupItem,
                                           tr("audit"),
                                           "schema",
                                           tr("本地连接/simple_db/架构/audit"));

    auto *publicTablesItem = createTreeItem(publicSchemaItem,
                                            tr("表"),
                                            "table_group",
                                            tr("本地连接/simple_db/架构/public/表"));
    auto *publicViewsItem = createTreeItem(publicSchemaItem,
                                           tr("视图"),
                                           "view_group",
                                           tr("本地连接/simple_db/架构/public/视图"));
    auto *publicFunctionsItem = createTreeItem(publicSchemaItem,
                                               tr("函数"),
                                               "function_group",
                                               tr("本地连接/simple_db/架构/public/函数"));
    auto *auditTablesItem = createTreeItem(auditSchemaItem,
                                           tr("表"),
                                           "table_group",
                                           tr("本地连接/simple_db/架构/audit/表"));

    createTreeItem(publicTablesItem,
                   tr("user_profile"),
                   "table",
                   tr("本地连接/simple_db/架构/public/表/user_profile"));
    createTreeItem(publicTablesItem,
                   tr("orders"),
                   "table",
                   tr("本地连接/simple_db/架构/public/表/orders"));
    createTreeItem(publicTablesItem,
                   tr("inventory_snapshot"),
                   "table",
                   tr("本地连接/simple_db/架构/public/表/inventory_snapshot"));
    createTreeItem(publicViewsItem,
                   tr("active_user_view"),
                   "view",
                   tr("本地连接/simple_db/架构/public/视图/active_user_view"));
    createTreeItem(publicViewsItem,
                   tr("daily_sales_view"),
                   "view",
                   tr("本地连接/simple_db/架构/public/视图/daily_sales_view"));
    createTreeItem(publicFunctionsItem,
                   tr("refresh_cache()"),
                   "function",
                   tr("本地连接/simple_db/架构/public/函数/refresh_cache()"));

    createTreeItem(auditTablesItem,
                   tr("system_log"),
                   "table",
                   tr("本地连接/simple_db/架构/audit/表/system_log"));
    createTreeItem(auditTablesItem,
                   tr("operation_record"),
                   "table",
                   tr("本地连接/simple_db/架构/audit/表/operation_record"));

    auto *analyticsSchemaGroupItem = createTreeItem(analyticsDatabaseItem,
                                                    tr("架构"),
                                                    "schema_group",
                                                    tr("本地连接/analytics_lab/架构"));
    auto *martSchemaItem = createTreeItem(analyticsSchemaGroupItem,
                                          tr("mart"),
                                          "schema",
                                          tr("本地连接/analytics_lab/架构/mart"));
    auto *martTablesItem = createTreeItem(martSchemaItem,
                                          tr("表"),
                                          "table_group",
                                          tr("本地连接/analytics_lab/架构/mart/表"));
    createTreeItem(martTablesItem,
                   tr("fact_sales"),
                   "table",
                   tr("本地连接/analytics_lab/架构/mart/表/fact_sales"));
    createTreeItem(martTablesItem,
                   tr("dim_store"),
                   "table",
                   tr("本地连接/analytics_lab/架构/mart/表/dim_store"));

    createTreeItem(favoriteItem,
                   tr("表 / user_profile"),
                   "favorite_item",
                   tr("常用入口/表 / user_profile"));
    createTreeItem(favoriteItem,
                   tr("视图 / active_user_view"),
                   "favorite_item",
                   tr("常用入口/视图 / active_user_view"));
    createTreeItem(recentItem,
                   tr("最近查询.sql"),
                   "recent_item",
                   tr("最近访问/最近查询.sql"));
    createTreeItem(recentItem,
                   tr("系统日志表"),
                   "recent_item",
                   tr("最近访问/系统日志表"));

    connectionItem->setExpanded(true);
    simpleDatabaseItem->setExpanded(true);
    schemaGroupItem->setExpanded(true);
    publicSchemaItem->setExpanded(true);
    publicTablesItem->setExpanded(true);
    analyticsDatabaseItem->setExpanded(true);
    analyticsSchemaGroupItem->setExpanded(true);
    favoriteItem->setExpanded(true);
    recentItem->setExpanded(true);

    if (directoryTree->topLevelItemCount() > 0) {
        updateCurrentPathLabel(directoryTree->topLevelItem(0));
    }
}

QTreeWidgetItem *DirectoryWidget::createTreeItem(QTreeWidgetItem *parent,
                                                 const QString &text,
                                                 const QString &nodeType,
                                                 const QString &nodePath)
{
    QTreeWidgetItem *item = nullptr;
    if (parent == nullptr) {
        item = new QTreeWidgetItem(directoryTree, {text});
    } else {
        item = new QTreeWidgetItem(parent, {text});
    }

    item->setData(0, NODE_TYPE_ROLE, nodeType);
    item->setData(0, NODE_PATH_ROLE, nodePath);
    applyItemStyle(item, nodeType, parent == nullptr || nodeType.endsWith("_group") || nodeType == "database" || nodeType == "schema");
    return item;
}

void DirectoryWidget::applyItemStyle(QTreeWidgetItem *item, const QString &nodeType, bool isBold)
{
    QColor textColor("#344054");
    QColor backgroundColor("#ffffff");

    if (nodeType == "connection") {
        textColor = QColor("#2563eb");
        backgroundColor = QColor("#eff6ff");
    } else if (nodeType == "database") {
        textColor = QColor("#7c3aed");
        backgroundColor = QColor("#f5f3ff");
    } else if (nodeType == "schema_group") {
        textColor = QColor("#8b5cf6");
        backgroundColor = QColor("#f8f5ff");
    } else if (nodeType == "schema") {
        textColor = QColor("#6366f1");
        backgroundColor = QColor("#eef2ff");
    } else if (nodeType == "table_group" || nodeType == "table") {
        textColor = QColor(nodeType == "table_group" ? "#059669" : "#047857");
        backgroundColor = QColor(nodeType == "table_group" ? "#ecfdf5" : "#f0fdf4");
    } else if (nodeType == "view_group" || nodeType == "view") {
        textColor = QColor(nodeType == "view_group" ? "#d97706" : "#b45309");
        backgroundColor = QColor("#fffbeb");
    } else if (nodeType == "function_group" || nodeType == "function") {
        textColor = QColor(nodeType == "function_group" ? "#db2777" : "#be185d");
        backgroundColor = QColor("#fdf2f8");
    } else if (nodeType == "favorite_group" || nodeType == "favorite_item") {
        textColor = QColor(nodeType == "favorite_group" ? "#ec4899" : "#be185d");
        backgroundColor = QColor(nodeType == "favorite_group" ? "#fdf2f8" : "#fff1f7");
    } else if (nodeType == "recent_group" || nodeType == "recent_item") {
        textColor = QColor(nodeType == "recent_group" ? "#0891b2" : "#0f766e");
        backgroundColor = QColor(nodeType == "recent_group" ? "#ecfeff" : "#f0fdfa");
    }

    QFont itemFont = item->font(0);
    itemFont.setBold(isBold);
    item->setFont(0, itemFont);
    item->setForeground(0, QBrush(textColor));
    item->setBackground(0, QBrush(backgroundColor));
}

void DirectoryWidget::updateCurrentPathLabel(QTreeWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    const QString nodePath = item->data(0, NODE_PATH_ROLE).toString();
    pathLabel->setText(tr("当前位置：%1").arg(nodePath));
}

void DirectoryWidget::filterDirectoryTree(const QString &keyword)
{
    for (int itemIndex = 0; itemIndex < directoryTree->topLevelItemCount(); ++itemIndex) {
        filterTreeItem(directoryTree->topLevelItem(itemIndex), keyword.trimmed());
    }
}

bool DirectoryWidget::filterTreeItem(QTreeWidgetItem *item, const QString &keyword)
{
    if (item == nullptr) {
        return false;
    }

    bool hasVisibleChild = false;
    for (int childIndex = 0; childIndex < item->childCount(); ++childIndex) {
        hasVisibleChild = filterTreeItem(item->child(childIndex), keyword) || hasVisibleChild;
    }

    const bool isKeywordEmpty = keyword.isEmpty();
    const bool isSelfMatched = item->text(0).contains(keyword, Qt::CaseInsensitive);
    const bool shouldShow = isKeywordEmpty || isSelfMatched || hasVisibleChild;

    item->setHidden(!shouldShow);
    if (!isKeywordEmpty && hasVisibleChild) {
        item->setExpanded(true);
    }

    return shouldShow;
}

void DirectoryWidget::showContextMenu(const QPoint &position)
{
    QTreeWidgetItem *item = directoryTree->itemAt(position);
    if (item == nullptr) {
        return;
    }

    directoryTree->setCurrentItem(item);
    updateCurrentPathLabel(item);

    QMenu menu(this);
    const QString nodeType = item->data(0, NODE_TYPE_ROLE).toString();
    const QString nodePath = item->data(0, NODE_PATH_ROLE).toString();

    emit requestDirectoryContextMenu(&menu, nodeType, nodePath);

    if (menu.actions().isEmpty()) {
        QAction *reservedAction = menu.addAction(tr("功能菜单预留接口"));
        reservedAction->setEnabled(false);
    }

    menu.exec(directoryTree->viewport()->mapToGlobal(position));
}
