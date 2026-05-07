/**
 * @file AiPanelWidget.h
 * @author YuzhSong
 * @brief 右侧 AI 面板占位组件头文件
 * @details 提供当前阶段的 AI 助手占位 UI，不实现任何真实 AI 业务功能
 * @module ui/opePanel
 */

#pragma once

#include <QWidget>

class QLabel;
class QVBoxLayout;

/**
 * @class AiPanelWidget
 * @author YuzhSong
 * @brief 右侧 AI 面板占位组件
 * @details
 * 1. 负责右侧 AI 区域占位显示。
 * 2. 不负责网络请求、SQL 解释、报错分析等业务逻辑。
 * 3. 由 OpePanelWidget 控制其显示/隐藏与布局宽度。
 */
class AiPanelWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     */
    explicit AiPanelWidget(QWidget* parent = nullptr);

private:
    /**
     * @brief 初始化界面结构
     */
    void initUI();

    /**
     * @brief 初始化样式
     */
    void initStyle();

private:
    QLabel* titleLabel;        ///< 标题标签
    QLabel* placeholderLabel;  ///< 占位主文案
    QLabel* hintLabel;         ///< 后续能力提示文案
    QVBoxLayout* mainLayout;   ///< 主布局
};

