/**
 * @file DirectoryWidget.h
 * @author YuzhSong
 * @brief 左侧数据库目录树面板头文件
 * @details 负责展示数据库对象树（数据库/表/字段等），不负责 SQL 编辑器与网络通信实现
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>
#include <QMap>
#include <cstdint>
#include <vector>

class QLabel;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QVariant;
class DatabaseNode;

/**
 * @class DirectoryWidget
 * @author YuzhSong
 * @brief 数据库目录树侧边面板
 * @details
 * 1. 负责展示类似 IDE Database Explorer 的数据库对象树。
 * 2. 当前阶段使用前端模拟数据，后续通过 refreshDirectory(...) 接入服务端真实元数据。
 * 3. 不负责“已打开文件列表”、SQL 执行、网络请求等无关逻辑。
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
     * @details
     * 1. 函数用途：用于切换连接、刷新前的树结构重置。
     * 2. 参数含义：无参数。
     * 3. 对目录树状态影响：会清空当前 QTreeWidget 的全部节点。
     * 4. 预留接口说明：否，属于当前可用功能。
     * @author YuzhSong
     */
    void clearDirectory();

    /**
     * @brief 加载模拟目录树数据
     * @details
     * 1. 函数用途：在无服务端元数据时展示可用的目录树结构。
     * 2. 参数含义：无参数。
     * 3. 对目录树状态影响：会先清空目录树，再重建模拟节点。
     * 4. 预留接口说明：否，当前用于联调与演示。
     * @author YuzhSong
     */
    void loadMockDirectory();

    /**
     * @brief 请求刷新目录树（前端占位入口）
     * @details
     * 1. 函数用途：响应 Refresh 按钮点击，触发刷新请求流程。
     * 2. 参数含义：无参数。
     * 3. 对目录树状态影响：当前阶段会触发本地模拟刷新（可见树结构重建）。
     * 4. 预留接口说明：作者：YuzhSong，此函数不做真实网络请求，后续由外部服务端模块接管数据拉取。
     * @author YuzhSong
     */
    void requestRefreshDirectory();

    /**
     * @brief 根据外部数据刷新目录树（简化预留接口）
     * @details
     * 1. 函数用途：接收外部模块给出的数据库列表并刷新目录树。
     * 2. 参数含义：databaseNames 为数据库名称列表。
     * 3. 对目录树状态影响：会清空后重建目录树。
     * 4. 预留接口说明：作者：YuzhSong，当前仅预留简单版本，后续应替换为数据库元数据模型结构。
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

    /**
     * @brief 获取指定数据库的版本号
     * @param dbName 数据库名称
     * @return 版本号，不存在时返回 0
     * @author NAPH130
     */
    std::uint64_t getDbVersion(const QString& dbName) const;

    /**
     * @brief 设置指定数据库的版本号
     * @param dbName 数据库名称
     * @param version 版本号
     * @author NAPH130
     */
    void setDbVersion(const QString& dbName, std::uint64_t version);

    void refreshTheme();

signals:
    /**
     * @brief 表节点激活信号
     * @details 用户双击表节点时发出，用于后续接入 SQL 生成或表结构查看
     * @param databaseName 所属数据库名
     * @param tableName 表名
     * @author YuzhSong
     */
    void tableActivated(const QString& databaseName, const QString& tableName);

    /**
     * @brief 字段节点激活信号
     * @details 用户双击字段节点时发出，用于后续接入列信息查看或 SQL 片段插入
     * @param databaseName 所属数据库名
     * @param tableName 所属表名
     * @param columnName 字段名
     * @author YuzhSong
     */
    void columnActivated(const QString& databaseName, const QString& tableName, const QString& columnName);

    /**
     * @brief 刷新目录请求信号
     * @details
     * 1. 函数用途：把“用户请求刷新目录”的动作向外部模块广播。
     * 2. 参数含义：无参数。
     * 3. 对目录树状态影响：信号本身不直接修改目录树状态。
     * 4. 预留接口说明：作者：YuzhSong，后续 OpePanelWidget/网络模块可连接该信号并发起真实服务端请求。
     * @author YuzhSong
     */
    void refreshRequested();

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
     * @brief 处理节点双击事件
     * @details 根据节点类型发出 tableActivated 或 columnActivated
     * @param item 被双击的树节点
     * @author YuzhSong
     */
    void handleItemDoubleClicked(QTreeWidgetItem* item);

private:
    QLabel* titleLabel;          ///< 作者：YuzhSong，顶部标题标签（位于左侧）
    QPushButton* refreshButton;  ///< 作者：YuzhSong，顶部 Refresh 按钮（位于右侧）
    QTreeWidget* directoryTree;  ///< 数据库对象树控件
    QMap<QString, std::uint64_t> dbVersionMap;  ///< 作者：NAPH130，数据库版本号映射表
};
