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
    void addOpenedFile(const QString& fileKey, const QString& displayName, bool modified);

    /**
     * @brief 设置当前文件选中项
     * @details 仅更新列表选中状态，不读取文件内容。
     * @param filePath 文件完整路径
     * @author YuzhSong
     */
    void setCurrentFile(const QString& fileKey);

    /**
     * @brief 更新文件未保存状态并刷新显示文本
     * @param fileKey 文件唯一标识（已落盘可用绝对路径，未落盘可用临时 key）
     * @param modified 是否未保存修改
     * @author YuzhSong
     */
    void updateFileModifiedState(const QString& fileKey, bool modified);

    /**
     * @brief 删除已打开文件项
     * @param fileKey 文件唯一标识
     * @author YuzhSong
     */
    void removeOpenedFile(const QString& fileKey);

    /**
     * @brief 根据旧 key 更新为新 key（用于未落盘文件另存为后转为真实路径）
     * @param oldFileKey 旧文件唯一标识
     * @param newFileKey 新文件唯一标识
     * @param newDisplayName 新显示名
     * @param modified 新未保存状态
     * @author YuzhSong
     */
    void updateFileKey(const QString& oldFileKey, const QString& newFileKey, const QString& newDisplayName, bool modified);

    /**
     * @brief 根据文件唯一标识查找列表项
     * @param fileKey 文件唯一标识
     * @return 命中返回列表项指针，否则返回 nullptr
     * @author YuzhSong
     */
    QListWidgetItem* findFileItem(const QString& fileKey) const;

    void refreshTheme();

signals:
    /**
     * @brief 文件激活信号
     * @details 用户点击/双击列表项时发出，供外部切换编辑器文件。
     * @param filePath 被激活文件的完整路径
     * @author YuzhSong
     */
    void fileActivated(const QString& fileKey);

    /**
     * @brief 请求关闭文件信号
     * @details FileWidget 只负责发起关闭请求，不负责保存判断与读写
     * @param fileKey 待关闭文件唯一标识
     * @author YuzhSong
     */
    void closeFileRequested(const QString& fileKey);

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
    /**
     * @brief 根据显示名和未保存状态组装列表展示文本
     * @param displayName 文件显示名
     * @param modified 是否未保存
     * @return 展示文本（未保存状态带 *）
     * @author YuzhSong
     */
    QString buildDisplayText(const QString& displayName, bool modified) const;

    /**
     * @brief 初始化文件列表右键菜单
     * @author YuzhSong
     */
    void setupContextMenu();

private:
    QListWidget* openedFileList;  ///< 已打开文件列表控件
};
