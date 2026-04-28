#pragma once

#include <QPoint>
#include <QWidget>

class MainWindow;
class QLineEdit;
class QLabel;
class QMenu;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * @class DirectoryWidget
 * @brief 目录树组件类
 * @details 用于展示数据库或项目文件层级结构
 * @author NAPH1301
 */
class DirectoryWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author NAPH1301
     * @param mainWindow 主窗口指针，用于后续页面跳转或操作
     * @param parent 父部件指针
     */
    explicit DirectoryWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author NAPH1301
     * @return 主窗口指针
     */
    MainWindow *getMainWindow() const;

signals:
    /**
     * @brief 请求构建目录节点右键菜单
     * @author NAPH1301
     * @param menu 待填充的菜单对象
     * @param nodeType 当前节点类型
     * @param nodePath 当前节点路径
     */
    void requestDirectoryContextMenu(QMenu *menu, const QString &nodeType, const QString &nodePath);

private:
    /**
     * @brief 初始化目录树结构
     * @author NAPH1301
     */
    void initializeDirectoryTree();

    /**
     * @brief 创建带类型和路径信息的目录节点
     * @author NAPH1301
     * @param parent 父节点
     * @param text 显示文本
     * @param nodeType 节点类型
     * @param nodePath 节点路径
     * @return 新创建的目录节点
     */
    QTreeWidgetItem *createTreeItem(QTreeWidgetItem *parent,
                                    const QString &text,
                                    const QString &nodeType,
                                    const QString &nodePath);

    /**
     * @brief 应用目录节点配色样式
     * @author NAPH1301
     * @param item 目标节点
     * @param nodeType 节点类型
     * @param isBold 是否加粗
     */
    void applyItemStyle(QTreeWidgetItem *item, const QString &nodeType, bool isBold = false);

    /**
     * @brief 根据节点更新当前位置文本
     * @author NAPH1301
     * @param item 当前节点
     */
    void updateCurrentPathLabel(QTreeWidgetItem *item);

    /**
     * @brief 根据关键字过滤目录节点
     * @author NAPH1301
     * @param keyword 搜索关键字
     */
    void filterDirectoryTree(const QString &keyword);

    /**
     * @brief 递归过滤目录节点
     * @author NAPH1301
     * @param item 当前节点
     * @param keyword 搜索关键字
     * @return 节点是否应显示
     */
    bool filterTreeItem(QTreeWidgetItem *item, const QString &keyword);

    /**
     * @brief 展示目录节点右键菜单
     * @author NAPH1301
     * @param position 菜单弹出位置
     */
    void showContextMenu(const QPoint &position);

    MainWindow *mainWindow; // 主窗口引用
    QLabel *pathLabel; // 当前路径展示
    QLineEdit *searchEdit; // 目录搜索框
    QTreeWidget *directoryTree; // 数据库目录树
};
