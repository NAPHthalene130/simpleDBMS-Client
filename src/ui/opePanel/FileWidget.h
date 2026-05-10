/**
 * @file FileWidget.h
 * @author YuzhSong
 * @brief 左侧已打开文件列表面板头文件
 * @details 负责展示“已打开 SQL 文件列表”并发出文件切换信号，不负责文件读写与 SQL 执行逻辑
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QListWidget;
class QListWidgetItem;

/**
 * @class FileWidget
 * @author YuzhSong
 * @brief 已打开文件列表面板
 * @details
 * 1. 负责展示已打开文件列表并维护当前选中项。
 * 2. 不负责读取文件内容、不负责保存文件、不负责编辑器切换执行。
 * 3. 由 OpePanelWidget 连接其信号到 EditorWidget，保持组件解耦。
 */
class FileWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit FileWidget(QWidget* parent = nullptr);

    /**
     * @brief 添加已打开文件
     * @details
     * 1. 不重复添加同一路径。
     * 2. 列表展示文件名，完整路径存储到 Qt::UserRole。
     * 3. 新增或命中已有项后会选中该项。
     * @param filePath 文件完整路径
     * @author YuzhSong
     */
    void addOpenedFile(const QString& filePath);

    /**
     * @brief 设置当前文件选中项
     * @details 仅更新列表选中状态，不读取文件内容。
     * @param filePath 文件完整路径
     * @author YuzhSong
     */
    void setCurrentFile(const QString& filePath);

signals:
    /**
     * @brief 文件激活信号
     * @details 用户点击/双击列表项时发出，供外部切换编辑器文件。
     * @param filePath 被激活文件的完整路径
     * @author YuzhSong
     */
    void fileActivated(const QString& filePath);

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
     * @brief 按路径查找文件列表项
     * @param filePath 文件完整路径
     * @return 命中则返回项指针，否则返回 nullptr
     * @author YuzhSong
     */
    QListWidgetItem* findFileItem(const QString& filePath) const;

    /**
     * @brief 规范化路径
     * @details 用于避免相对路径、分隔符差异导致的重复项。
     * @param filePath 原始路径
     * @return 规范化后的绝对路径
     * @author YuzhSong
     */
    QString normalizeFilePath(const QString& filePath) const;

    /**
     * @brief 判断两个路径是否表示同一文件
     * @param leftPath 左路径
     * @param rightPath 右路径
     * @return 相同返回 true，否则 false
     * @author YuzhSong
     */
    bool isSameFilePath(const QString& leftPath, const QString& rightPath) const;

private:
    QListWidget* openedFileList;  ///< 已打开文件列表控件
};

