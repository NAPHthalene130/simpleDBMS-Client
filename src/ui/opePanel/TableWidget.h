#pragma once

#include <QWidget>
#include <map>
#include <string>
#include <utility>
#include <vector>

class DatabaseNode;
class MainWindow;
class QLabel;
class QPushButton;
class QTableWidget;
class QString;

/**
 * @struct CellChange
 * @brief 单元格修改记录
 * @details 记录一次单元格值变更的元数据，用于生成 UPDATE SQL
 * @author Qi
 */
struct CellChange {
    int rowIndex;            ///< 行索引
    int colIndex;            ///< 列索引
    QString columnName;      ///< 列名
    QString oldValue;        ///< 旧值
    QString newValue;        ///< 新值
    QString pkColumnName;    ///< 主键列名
    QString pkValue;         ///< 该行主键值
};

/**
 * @class TableWidget
 * @brief 数据表展示组件类
 * @details 用于展示数据库查询结果或表结构，支持单元格编辑、插入行、删除行与 UPDATE/INSERT/DELETE 提交
 * @author Qi
 */
class TableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    MainWindow *getMainWindow() const;

    /**
     * @brief 设置查询结果并识别可编辑上下文
     * @author Qi
     * @param columns 查询结果列名列表
     * @param rows 查询结果二维数据
     * @param tableName 表名
     * @param dbName 数据库名
     * @param directoryData 服务端目录数据（用于查找表结构及主键）
     */
    void setQueryResult(const std::vector<std::string> &columns,
                        const std::vector<std::vector<std::string>> &rows,
                        const std::string &tableName = std::string(),
                        const std::string &dbName = std::string(),
                        const std::vector<DatabaseNode> &directoryData = {});

    void clearTable();
    void showMessage(const QString &message);
    void refreshTheme();

signals:
    void backToEditorRequested();
    void sqlExecutionRequested(const QString &sql);

private slots:
    void onCellChanged(int row, int col);
    void onSubmitClicked();
    void onCustomContextMenu(const QPoint &pos);

private:
    void rebuildTableView();
    void applyTableStyle();
    void applyEditRestrictions();

    bool tryDetectPrimaryKey(const std::vector<DatabaseNode> &directoryData,
                             const std::string &tableName,
                             QString &outPkColumnName);

    /**
     * @brief 检测当前查询是否为 SELECT *（列数与目录匹配）
     * @author Qi
     */
    bool isSelectAllQuery(const std::vector<DatabaseNode> &directoryData) const;

    /**
     * @brief 获取插入行的当前值列表
     * @author Qi
     * @return 插入行各列的值
     */
    QStringList collectInsertRowValues() const;

    /**
     * @brief 确认值不为空，空则弹提示并恢复
     * @author Qi
     * @return 是否通过
     */
    bool validateNotEmpty();

    static constexpr int kInsertRowIndex = 0;
    static constexpr int kDataOffset = 1;

    MainWindow *mainWindow;
    QLabel *messageLabel;
    QPushButton *backButton;
    QPushButton *submitButton;
    QTableWidget *tableViewWidget;

    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    std::string tableName;
    std::string dbName;
    QString primaryKeyColumn;

    bool isEditingAllowed;
    bool isSelectAll;
    bool hasInsertRow;
    bool isContentLoading;
    std::map<std::pair<int, int>, CellChange> pendingChanges;
};
