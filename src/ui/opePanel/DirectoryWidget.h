/**
 * @file DirectoryWidget.h
 * @brief 左侧目录与已打开文件列表组件头文件
 * @details 该组件仅负责左侧导航区域的 UI 展示与交互，不负责文件读写与编辑器内容管理。
 *          通过信号 fileActivated 将“用户选择了哪个文件”通知外部协调者，实现组件解耦。
 * @author YuzhSong
 * @date 2026-04-26
 */

#pragma once

#include <QWidget>

class QListWidget;
class QListWidgetItem;
class MainWindow;

/**
 * @class DirectoryWidget
 * @brief 左侧导航组件
 * @details 负责显示“已打开文件”列表，支持外部添加文件、切换当前文件、响应点击并发出激活信号。
 *          该类不读取文件内容，不保存文件，不运行 SQL。
 * @author YuzhSong
 */
class DirectoryWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param mainWindow 主窗口指针
     * @param parent 父组件指针
     */
    explicit DirectoryWidget(MainWindow* mainWindow, QWidget* parent = nullptr);

    /**
     * @brief 获取主窗口指针
     * @author YuzhSong
     * @return 主窗口指针
     */
    MainWindow* getMainWindow() const;

    /**
     * @brief 添加已打开文件到列表
     * @details
     * 1. 空路径不添加
     * 2. 已存在路径不重复添加
     * 3. 新增后自动选中对应项
     * 4. 列表显示文件名，完整路径存储在 UserRole
     * @author YuzhSong
     * @param filePath 文件完整路径
     */
    void addOpenedFile(const QString& filePath);

    /**
     * @brief 设置当前文件选中项
     * @details 根据完整路径在列表中匹配并选中；不负责加载文件内容。
     * @author YuzhSong
     * @param filePath 文件完整路径
     */
    void setCurrentFile(const QString& filePath);

signals:
    /**
     * @brief 文件激活信号
     * @details 当用户点击或双击列表项时发出，通知外部切换编辑器文件。
     * @author YuzhSong
     * @param filePath 被激活文件的完整路径
     */
    void fileActivated(const QString& filePath);

private:
    /**
     * @brief 初始化 UI
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
     * @details 仅负责目录面板视觉风格设置，不影响文件列表信号槽与业务行为。
     * @author YuzhSong
     */
    void initStyle();

    /**
     * @brief 按路径查找列表项
     * @author YuzhSong
     * @param filePath 文件完整路径
     * @return 命中返回项指针，否则返回 nullptr
     */
    QListWidgetItem* findFileItem(const QString& filePath) const;

    /**
     * @brief 规范化路径
     * @details 统一路径格式用于去重与匹配，避免相对路径/分隔符差异导致重复项。
     * @author YuzhSong
     * @param filePath 原始路径
     * @return 规范化后的绝对路径
     */
    QString normalizeFilePath(const QString& filePath) const;

    /**
     * @brief 判断两个路径是否表示同一文件
     * @author YuzhSong
     * @param leftPath 左路径
     * @param rightPath 右路径
     * @return 相同返回 true，不同返回 false
     */
    bool isSameFilePath(const QString& leftPath, const QString& rightPath) const;

private:
    MainWindow* mainWindow;       ///< 主窗口引用
    QListWidget* openedFileList;  ///< 已打开文件列表控件
};
