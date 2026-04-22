/**
 * @file sqlEditor.h
 * @brief SQL 编辑器核心组件头文件
 * @details 继承 QPlainTextEdit，集成行号显示、当前行高亮、自动缩进和 SQL 自动补全功能。
 *          该类是编辑区的核心，负责处理编辑行为和视觉呈现。
 *          自动补全功能使用 QCompleter 实现，支持关键字提示、不区分大小写匹配和键盘交互。
 * @author YuzhSong
 * @date 2026-04-11
 */

#pragma once

#include <QPlainTextEdit>
#include <QRectF>

class LineNumberArea;
class SqlHighlighter;
class QCompleter;
class QFocusEvent;

/**
 * @class SqlEditor
 * @brief SQL 编辑器类
 * @details 继承自 QPlainTextEdit，扩展了以下功能：
 *          - 行号显示（通过 LineNumberArea）
 *          - 当前行高亮
 *          - 自动缩进（回车时继承上一行缩进）
 *          - 语法高亮（通过 SqlHighlighter）
 * @author YuzhSong
 */
class SqlEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param parent 父部件指针
     */
    explicit SqlEditor(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     * @author YuzhSong
     */
    ~SqlEditor();

    /**
     * @brief 获取行号区域宽度并调整编辑区边距
     * @details 由 LineNumberArea 调用，用于同步更新布局
     * @author YuzhSong
     */
    void lineNumberAreaWidthUpdate();

    /**
     * @brief 绘制行号区域
     * @details 触发行号区域的重新绘制
     * @author YuzhSong
     */
    void lineNumberAreaPaintEvent(QPaintEvent* event);

    /**
     * @brief 调整编辑器滚动区域边距
     * @details 为行号区域留出空间
     * @author YuzhSong
     */
    void updateEditorViewportMargins();

    /**
     * @brief 获取语法高亮器指针
     * @author YuzhSong
     * @return 语法高亮器指针
     */
    SqlHighlighter* getHighlighter() const;

protected:
    /**
     * @brief 重置事件处理函数
     * @details 当窗口大小变化时，更新行号区域
     * @author YuzhSong
     * @param event 重置事件对象
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief 按键按下事件处理函数
     * @details 处理 Tab 键、回车的自动缩进逻辑，以及自动补全的键盘交互。
     *          支持以下补全交互：
     *          - Up/Down：在候选列表中上下选择
     *          - Enter/Tab：完成补全并插入
     *          - Escape：关闭补全框
     *          - 其他字符键：输入时触发补全
     * @author YuzhSong
     * @param event 按键事件对象
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief 焦点进入事件处理函数
     * @details 当编辑器获得焦点时，绑定 QCompleter 以启用自动补全
     * @author YuzhSong
     * @param event 焦点事件对象
     */
    void focusInEvent(QFocusEvent* event) override;

    /**
     * @brief 提取光标所在位置的当前单词
     * @details 从光标位置向前扫描，提取由字母、数字和下划线组成的完整单词。
     *          用于自动补全的前缀匹配。
     * @author YuzhSong
     * @return 当前单词字符串
     */
    QString textUnderCursor() const;

    /**
     * @brief 插入补全内容到编辑器
     * @details 将选中的补全项替换当前正在输入的单词。
     *          使用 QTextCursor 进行文本替换，确保光标位置正确更新。
     * @author YuzhSong
     * @param completion 要插入的补全字符串
     */
    void insertCompletion(const QString& completion);

private slots:
    /**
     * @brief 更新请求处理槽函数
     * @details 处理编辑区的更新请求，同步刷新行号区域
     * @author YuzhSong
     * @param rect 更新区域
     * @param dy 垂直偏移量
     */
    void updateLineNumberArea(const QRect& rect, int dy);

    /**
     * @brief 块数量变化处理槽函数
     * @details 当文本块数量变化时，调整行号区域宽度
     * @author YuzhSong
     */
    void updateLineNumberAreaWidth(int newBlockCount);

    /**
     * @brief 光标位置变化处理槽函数
     * @details 光标移动时高亮当前行
     * @author YuzhSong
     */
    void highlightCurrentLine();

    /**
     * @brief 滚动条值变化处理槽函数
     * @details 滚动编辑器时同步滚动行号区域
     * @author YuzhSong
     */
    void onVerticalScrollBarValueChanged(int value);

private:
    /**
     * @brief 初始化行号区域
     * @details 创建 LineNumberArea 并建立信号槽连接
     * @author YuzhSong
     */
    void setupLineNumberArea();

    /**
     * @brief 初始化语法高亮
     * @details 创建 SqlHighlighter 并关联到本文档
     * @author YuzhSong
     */
    void setupSyntaxHighlighter();

    /**
     * @brief 初始化 SQL 自动补全器
     * @details 创建 QCompleter 并配置以下特性：
     *          - 使用 QStringListModel 管理关键字列表
     *          - 不区分大小写匹配（Qt::CaseInsensitive）
     *          - 弹出式补全框（完成时不自动插入）
     *          关键字包含 SQL 常用语法元素。
     * @author YuzhSong
     */
    void setupCompleter();

    /**
     * @brief 触发自动补全
     * @details 检查当前输入的单词是否满足补全条件（长度 >= 1），
     *          如果满足则弹出补全框并显示候选列表。
     *          使用 completer->complete(cursorRect()) 定位补全框位置。
     * @author YuzhSong
     */
    void triggerCompletion();

    /**
     * @brief 获取当前行的缩进字符串
     * @details 提取当前行首的空格和 Tab 字符
     * @author YuzhSong
     * @return 缩进字符串（包含空格和 Tab）
     */
    QString getCurrentLineIndentation() const;

    LineNumberArea* lineNumberArea;      ///< 行号显示区域
    SqlHighlighter* sqlHighlighter;      ///< SQL 语法高亮器
    QCompleter* sqlCompleter;            ///< SQL 自动补全器
};
