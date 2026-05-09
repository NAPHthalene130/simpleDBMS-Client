/**
 * @file LineNumberArea.cpp
 * @brief 行号显示区域组件实现文件
 * @details LineNumberArea 是轻量转发层，实际绘制逻辑由 SqlEditor 完成。
 *          这是因为访问 QPlainTextEdit 的 protected 成员需要继承关系。
 * @author YuzhSong
 * @date 2026-04-11
 */

#include "LineNumberArea.h"
#include "SqlEditor.h"

#include <QPainter>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param editor 关联的 SqlEditor 指针
 * @param parent 父部件指针
 */
LineNumberArea::LineNumberArea(SqlEditor* editor, QWidget* parent)
    : QWidget(parent)
    , sqlEditor(editor)
{
    // 设置背景色，与编辑器区分
    setAutoFillBackground(true);
}

/**
 * @brief 析构函数
 * @author YuzhSong
 */
LineNumberArea::~LineNumberArea()
{
    // Qt 自动管理子对象内存，无需额外清理
}

/**
 * @brief 获取行号区域所需宽度
 * @details 根据文档总行数和字体大小计算所需宽度
 * @author YuzhSong
 * @return 行号区域宽度（像素）
 */
int LineNumberArea::lineNumberAreaWidth() const
{
    // 获取文档总行数
    int digits = 1;
    int lineCount = sqlEditor->blockCount();

    // 计算最大行号需要的字符数
    int max = qMax(1, lineCount);
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 额外预留一些空间（行号 + 边距）
    int space = 12 + sqlEditor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

/**
 * @brief 绘制事件处理函数（转发给 SqlEditor）
 * @details LineNumberArea 本身不绘制，仅将事件转发给 SqlEditor
 * @author YuzhSong
 * @param event 绘制事件对象
 */
void LineNumberArea::paintEvent(QPaintEvent* event)
{
    // 转发给 SqlEditor，由它完成行号绘制（需要访问 protected 接口）
    sqlEditor->lineNumberAreaPaintEvent(event);
}
