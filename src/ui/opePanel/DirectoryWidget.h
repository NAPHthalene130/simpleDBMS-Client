/**
 * @file DirectoryWidget.h
 * @author YuzhSong
 * @brief 左侧数据库目录树面板头文件
 * @details 负责展示数据库对象树（数据库/表/字段等），不负责文件列表与 SQL 编辑执行逻辑
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>
#include <vector>

class QTreeWidget;
class QTreeWidgetItem;
class QVariant;
class DatabaseNode;

/**
 * @class DirectoryWidget
 * @author YuzhSong
 * @brief 数据库目录树侧边面板
 * @details
 * 1. 负责展示类似 DataGrip/JetBrains Database Explorer 的数据库对象树。
 * 2. 当前阶段使用前端模拟数据，后续通过 refreshDirectory 接入服务端元数据。
 * 3. 不负责“已打开文件列表”，该职责已迁移到 FileWidget。
 */
class DirectoryWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit DirectoryWidget(QWidget* parent = nullptr);

    /**
     * @brief 清空目录树
     * @details 用于切换连接、刷新元数据前重置界面。
     * @author YuzhSong
     */
    void clearDirectory();

    /**
     * @brief 加载模拟目录树数据
     * @details 当前阶段仅做前端占位，后续会由真实元数据刷新替换。
     * @author YuzhSong
     */
    void loadMockDirectory();

    /**
     * @brief 根据外部数据刷新目录树（简化预留接口）
     * @details
     * 1. 当前使用数据库名列表做轻量演示。
     * 2. 后续将扩展为接收服务端返回的完整数据库/表/字段元数据结构。
     * @param databaseNames 数据库名称列表
     * @author YuzhSong
     */
    void refreshDirectory(const QStringList& databaseNames);

    /**
     * @brief 根据服务端返回的完整目录结构刷新目录树
     * @details
     * 1. 接收服务端 DIRECTORY_RESPONSE 返回的 库-表-字段 层级数据。
     * 2. 清空现有树结构后按 DatabaseNode → TableNode → fields 重建目录树。
     * @param databases 服务端返回的数据库目录结构
     * @author NAPH130
     */
    void refreshFromServer(const std::vector<DatabaseNode>& databases);

    void refreshTheme();

signals:
    /**
     * @brief 表节点激活信号
     * @details 用户双击表节点时发出，后续可用于 SQL 生成或表结构查看。
     * @param databaseName 所属数据库名
     * @param tableName 表名
     * @author YuzhSong
     */
    void tableActivated(const QString& databaseName, const QString& tableName);

    /**
     * @brief 字段节点激活信号
     * @details 用户双击字段节点时发出，后续可用于列信息查看或 SQL 片段插入。
     * @param databaseName 所属数据库名
     * @param tableName 所属表名
     * @param columnName 字段名
     * @author YuzhSong
     */
    void columnActivated(const QString& databaseName, const QString& tableName, const QString& columnName);

private:
    /**
     * @brief 初始化 UI 结构
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化信号槽连接
     * @author YuzhSong
     */
    void initConnections();

    /**
     * @brief 初始化样式
     * @author YuzhSong
     */
    void initStyle();

    /**
     * @brief 创建数据库节点
     * @param root 根节点
     * @param databaseName 数据库名
     * @return 数据库节点指针
     * @author YuzhSong
     */
    QTreeWidgetItem* createDatabaseNode(QTreeWidgetItem* root, const QString& databaseName);

    /**
     * @brief 创建表节点
     * @param tablesNode Tables 分类节点
     * @param databaseName 数据库名
     * @param tableName 表名
     * @param columns 字段显示文本列表（如 "id : INT"）
     * @author YuzhSong
     */
    void createTableNode(QTreeWidgetItem* tablesNode,
                         const QString& databaseName,
                         const QString& tableName,
                         const QStringList& columns);

    /**
     * @brief 处理树节点双击事件
     * @details 根据节点类型判断发出 tableActivated 或 columnActivated。
     * @param item 被双击节点
     * @author YuzhSong
     */
    void handleItemDoubleClicked(QTreeWidgetItem* item);

private:
    QTreeWidget* directoryTree;  ///< 数据库对象树控件
};

