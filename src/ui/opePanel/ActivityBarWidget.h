/**
 * @file ActivityBarWidget.h
 * @author YuzhSong
 * @brief IDE 左侧 ActivityBar 头文件
 * @details 提供 File / Log / AI / Terminal 按钮与状态同步接口，不承载面板业务逻辑
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QPushButton;

/**
 * @class ActivityBarWidget
 * @author YuzhSong
 * @brief 左侧 ActivityBar 组件
 * @details
 * 1. 负责发出“请求切换面板”的信号。
 * 2. 负责展示按钮选中态。
 * 3. 不负责任何面板的创建、布局和显示/隐藏决策。
 */
class ActivityBarWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     */
    explicit ActivityBarWidget(QWidget* parent = nullptr);

    /**
     * @brief 同步 File 按钮选中状态
     * @param checked true 表示 File 面板打开
     */
    void setFileChecked(bool checked);

    /**
     * @brief 同步 Log 按钮选中状态
     * @param checked true 表示 Log 面板打开
     */
    void setLogChecked(bool checked);

    /**
     * @brief 同步 AI 按钮选中状态
     * @param checked true 表示 AI 面板打开
     */
    void setAiChecked(bool checked);

    /**
     * @brief 同步 Terminal 按钮选中状态
     * @param checked true 表示 Terminal 面板打开
     */
    void setTerminalChecked(bool checked);

    /**
     * @brief 批量同步四个按钮状态
     * @param fileActive File 按钮状态
     * @param logActive Log 按钮状态
     * @param aiActive AI 按钮状态
     * @param terminalActive Terminal 按钮状态
     */
    void updateSelectionState(bool fileActive, bool logActive, bool aiActive, bool terminalActive);

signals:
    /**
     * @brief 请求切换 File 面板
     */
    void filePanelRequested();

    /**
     * @brief 请求切换 Log 面板
     */
    void logPanelRequested();

    /**
     * @brief 请求切换 AI 面板
     */
    void aiPanelRequested();

    /**
     * @brief 请求切换 Terminal 面板
     */
    void terminalPanelRequested();

private:
    /**
     * @brief 初始化界面结构
     */
    void initUI();

    /**
     * @brief 初始化样式
     */
    void initStyle();

    /**
     * @brief 初始化信号槽连接
     */
    void initConnections();

private:
    QPushButton* fileButton;      ///< File 按钮
    QPushButton* logButton;       ///< Log 按钮
    QPushButton* aiButton;        ///< AI 按钮
    QPushButton* terminalButton;  ///< Terminal 按钮
};

