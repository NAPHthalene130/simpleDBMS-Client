/**
 * @file LogWidget.h
 * @brief 左侧操作日志面板组件头文件
 * @details 提供本地操作日志显示占位能力，本阶段不接入真实日志采集链路。
 * @author YuzhSong
 * @date 2026-05-07
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QLabel;
class QListWidget;

/**
 * @class LogWidget
 * @brief 左侧日志面板组件
 * @details
 * 1. 负责展示本地操作日志文本列表。
 * 2. 不负责服务端返回输出，避免与 TerminalWidget 角色混淆。
 * 3. 由 SidePanelContainer 持有并参与面板切换。
 * @author YuzhSong
 */
class LogWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     * @author YuzhSong
     */
    explicit LogWidget(QWidget* parent = nullptr);

    /**
     * @brief 追加日志
     * @details 向日志列表末尾添加一条消息，用于后续外部接入本地操作日志。
     * @param message 日志文本
     * @author YuzhSong
     */
    void appendLog(const QString& message);

private:
    /**
     * @brief 初始化界面
     * @author YuzhSong
     */
    void initUI();

    /**
     * @brief 初始化样式
     * @author YuzhSong
     */
    void initStyle();

private:
    QLabel* titleLabel;      ///< 标题标签
    QListWidget* logList;    ///< 日志列表
};

