/**
 * @file lineNumberArea.h
 * @brief 行号显示区域组件头文件
 * @details 为 SqlEditor 提供行号显示功能，集成到编辑器左侧。
 *          该类仅作为轻量转发层，实际绘制逻辑在 SqlEditor 中完成
 *          （因为需要访问 QPlainTextEdit 的 protected 成员函数）。
 * @author YuzhSong
 * @date 2026-04-11
 */

#pragma once

#include <QWidget>

class SqlEditor;

/**
 * @class LineNumberArea
 * @brief 行号显示区域类（轻量转发层）
 * @details 继承自 QWidget，作为 SqlEditor 的行号边栏。
 *          自身不包含绘制逻辑，paintEvent 直接转发给 SqlEditor。
 *          这样做是因为 QPlainTextEdit 的 firstVisibleBlock()、
 *          blockBoundingGeometry() 等函数是 protected，只能在子类中访问。
 * @author YuzhSong
 */
class LineNumberArea : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param editor 关联的 SqlEditor 指针
     * @param parent 父部件指针
     */
    explicit LineNumberArea(SqlEditor* editor, QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     * @author YuzhSong
     */
    ~LineNumberArea();

    /**
     * @brief 获取行号区域所需宽度
     * @details 根据当前文档行数计算宽度，确保能显示最大行号
     * @author YuzhSong
     * @return 行号区域宽度（像素）
     */
    int lineNumberAreaWidth() const;

protected:
    /**
     * @brief 绘制事件处理函数（转发给 SqlEditor）
     * @author YuzhSong
     * @param event 绘制事件对象
     */
    void paintEvent(QPaintEvent* event) override;

private:
    SqlEditor* sqlEditor;  ///< 关联的 SqlEditor 指针
};
