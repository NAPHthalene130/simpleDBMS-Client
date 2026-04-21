#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QMenu>

// 节点类型枚举（存储在 UserRole+1）
enum ItemType { TypeServer = 1000, TypeDatabase, TypeTable, TypeColumn };

// 自定义树控件，使用模拟数据填充
class DatabaseTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DatabaseTreeWidget(QWidget *parent = nullptr);
    void setConnectionInfo(const QString &host, int port, const QString &user);
    void loadDatabases();
    void refreshCurrentNode();

    signals:
        void openTableRequested(const QString &database, const QString &table);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onItemExpanded(QTreeWidgetItem *item);
    void onCustomContextMenu(const QPoint &pos);
    void onRefresh();
    void onOpenTable();

private:
    void loadTables(QTreeWidgetItem *dbItem);
    void loadColumns(QTreeWidgetItem *tableItem);
    QString databaseFromItem(QTreeWidgetItem *item) const;
    QString tableFromItem(QTreeWidgetItem *item) const;

    QMenu *m_contextMenu;
    QString m_host;
    int m_port;
    QString m_user;
};

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(const QString &host, int port,
                           const QString &user, const QString &password,
                           QWidget *parent = nullptr);
    ~ContentWidget();

private slots:
    void onExecuteQuery();
    void onOpenTable(const QString &database, const QString &table);

private:
    void setupUI();

    DatabaseTreeWidget *m_tree;
    QTextEdit *m_queryEditor;
    QTextEdit *m_resultView;

    QString m_host;
    int m_port;
    QString m_user;
};

#endif // CONTENTWIDGET_H