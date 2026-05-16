/**
 * @file SqlEditor.cpp
 * @brief SQL 编辑器核心组件实现文件
 * @details 实现行号集成、当前行高亮和自动缩进功能。
 * @author YuzhSong
 * @date 2026-04-11
 */

#include "SqlEditor.h"

#include "LineNumberArea.h"
#include "SqlHighlighter.h"
#include "ui/ThemeManager.h"

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QCompleter>
#include <QFocusEvent>
#include <QFont>
#include <QKeyEvent>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>
#include <QTimer>
#include <QtGlobal>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param parent 父部件指针
 */
SqlEditor::SqlEditor(QWidget* parent)
    : QPlainTextEdit(parent)
    , lineNumberArea(nullptr)
    , sqlHighlighter(nullptr)
    , sqlCompleter(nullptr)
    , lineNumberVisible(true)
{
    // 初始化行号显示区域
    setupLineNumberArea();

    // 初始化语法高亮器
    setupSyntaxHighlighter();

    // 初始化 SQL 自动补全器
    setupCompleter();

    // 设置等宽字体，适合代码编辑
    QFont editorFont("Consolas", 12);
    setFont(editorFont);
    setStyleSheet(ThemeManager::sqlEditor());

    // 设置 Tab 宽度为 4 个空格
    setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));

    // 禁用自动换行，适合代码编辑
    setLineWrapMode(QPlainTextEdit::NoWrap);

    // 连接信号槽：文档更新时刷新行号
    connect(this, &QPlainTextEdit::updateRequest,
            this, &SqlEditor::updateLineNumberArea);

    // 连接信号槽：块数量变化时调整行号宽度
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &SqlEditor::updateLineNumberAreaWidth);

    // 连接信号槽：光标位置变化时高亮当前行
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &SqlEditor::highlightCurrentLine);

    // 连接信号槽：垂直滚动条变化时同步行号
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &SqlEditor::onVerticalScrollBarValueChanged);

    // 初始化行号宽度
    updateLineNumberAreaWidth(0);

    // 初始高亮当前行
    highlightCurrentLine();
}

/**
 * @brief 析构函数
 * @author YuzhSong
 */
SqlEditor::~SqlEditor()
{
    // Qt 自动管理子对象内存，无需额外清理
}

/**
 * @brief 初始化行号区域
 * @details 创建 LineNumberArea 并建立信号槽连接
 * @author YuzhSong
 */
void SqlEditor::setupLineNumberArea()
{
    // 创建行号区域组件，设置为子部件
    lineNumberArea = new LineNumberArea(this, this);

    // 连接行号区域的更新信号
    // updateRequest(const QRect&, int) 信号用于通知行号区域需要重绘
    // 使用 lambda 将信号转发到 lineNumberArea->update()
    connect(this, &QPlainTextEdit::updateRequest,
            this, [this](const QRect& rect, int dy) {
        if (dy) {
            lineNumberArea->scroll(0, dy);
        }
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    });
}

/**
 * @brief 初始化语法高亮
 * @details 创建 SqlHighlighter 并关联到本文档
 * @author YuzhSong
 */
void SqlEditor::setupSyntaxHighlighter()
{
    // 创建语法高亮器，自动关联到本文档
    // QSyntaxHighlighter 接受 QTextDocument* 作为父对象
    sqlHighlighter = new SqlHighlighter(this, document());
}

/**
 * @brief 初始化 SQL 自动补全器
 * @details 创建 QCompleter 并配置以下特性：
 *          - 使用 QStringListModel 管理关键字列表
 *          - 不区分大小写匹配（Qt::CaseInsensitive）
 *          - 弹出式补全框（完成时不自动插入）
 *          关键字包含 SQL 常用语法元素，包括：
 *          - SQL 语句关键字（SELECT, FROM, WHERE 等）
 *          - 数据类型（INT, VARCHAR 等）
 *          - 常用函数和子句
 * @author YuzhSong
 */
void SqlEditor::setupCompleter()
{
    // 创建 SQL 关键字列表
    // 包含常用的 SQL 语句关键字、数据类型和常用函数
    QStringList sqlKeywords;
    
    // SQL 语句关键字
    sqlKeywords << "SELECT" << "FROM" << "WHERE" << "INSERT" << "UPDATE" << "DELETE"
                << "CREATE" << "DROP"
                // 2026-04-22 修改：补充 SHOW 自动补全候选词，作者：YuzhSong。
                // 说明：沿用现有关键字列表驱动补全机制，确保输入 sh / sho / show 时可正常提示 SHOW。
                << "SHOW"
                << "ALTER" << "TABLE" << "INTO" << "VALUES"
                << "SET" << "AND" << "OR" << "NOT" << "NULL" << "IS"
                << "JOIN" << "LEFT" << "RIGHT" << "INNER" << "OUTER" << "ON"
                << "GROUP" << "BY" << "ORDER" << "HAVING" << "LIMIT" << "OFFSET"
                << "DISTINCT" << "AS" << "IN" << "BETWEEN" << "LIKE" << "EXISTS"
                << "UNION" << "ALL" << "ANY" << "SOME"
                << "DATABASE" << "DATABASES" << "TABLES"
                << "USE" << "PRIMARY KEY" << "UNIQUE" << "DEFAULT" << "FOREIGN" << "REFERENCES";
    
    // 数据类型
    sqlKeywords << "INT" << "INTEGER" << "VARCHAR" << "CHAR" << "TEXT"
                << "BLOB" << "REAL" << "FLOAT" << "DOUBLE" << "BOOLEAN"
                << "DATE" << "TIME" << "DATETIME" << "TIMESTAMP"
                << "DECIMAL" << "NUMERIC" << "SERIAL";
    
    // 常用函数
    sqlKeywords << "COUNT" << "SUM" << "AVG" << "MAX" << "MIN"
                << "UPPER" << "LOWER" << "LENGTH" << "TRIM" << "SUBSTR"
                << "COALESCE" << "CASE" << "WHEN" << "THEN" << "ELSE" << "END";
    
    // 创建补全器并设置数据模型
    sqlCompleter = new QCompleter(sqlKeywords, this);
    baseKeywords = sqlKeywords;

    // 设置不区分大小写匹配
    sqlCompleter->setCaseSensitivity(Qt::CaseInsensitive);

    // 设置为弹出式补全模式（由我们手动控制 insertCompletion）
    sqlCompleter->setCompletionMode(QCompleter::PopupCompletion);

    // 设置补全框最多显示 10 个候选项
    sqlCompleter->setMaxVisibleItems(10);

    // 设置补全框的最小宽度，确保能完整显示所有候选文本
    // 获取最长的关键字长度作为参考
    int maxWidth = 0;
    for (const QString& keyword : sqlKeywords) {
        int width = fontMetrics().horizontalAdvance(keyword);
        if (width > maxWidth) {
            maxWidth = width;
        }
    }
    // 设置补全框宽度为最长关键字的宽度加上额外边距（至少 200 像素）
    int popupWidth = qMax(maxWidth + 40, 200);
    sqlCompleter->popup()->setMinimumWidth(popupWidth);
    sqlCompleter->popup()->setFixedWidth(popupWidth);

    // 设置补全器绑定的 widget 为当前编辑器
    sqlCompleter->setWidget(this);
    
    // 连接补全器的 activated 信号到 insertCompletion 槽
    // 当用户按 Enter 或 Tab 时触发
    connect(sqlCompleter, QOverload<const QString&>::of(&QCompleter::activated),
            this, &SqlEditor::insertCompletion);
}

/**
 * @brief 获取语法高亮器指针
 * @author YuzhSong
 * @return 语法高亮器指针
 */
SqlHighlighter* SqlEditor::getHighlighter() const
{
    return sqlHighlighter;
}

/**
 * @brief 获取当前行的缩进字符串
 * @details 提取当前行首的空格和 Tab 字符，用于自动缩进
 * @author YuzhSong
 * @return 缩进字符串（包含空格和 Tab）
 */
QString SqlEditor::getCurrentLineIndentation() const
{
    // 获取当前光标所在文本块
    QTextBlock currentBlock = textCursor().block();
    QString text = currentBlock.text();

    // 提取行首的空白字符（空格和 Tab）
    QString indentation;
    for (const QChar& ch : text) {
        if (ch == ' ' || ch == '\t') {
            indentation += ch;
        } else {
            break;
        }
    }

    return indentation;
}

/**
 * @brief 提取光标所在位置的当前单词
 * @details 从光标位置向前扫描，提取由字母、数字和下划线组成的完整单词。
 *          使用 QTextCursor 的 Position 操作来精确定位单词边界。
 *          该函数用于自动补全的前缀匹配。
 * @author YuzhSong
 * @return 当前单词字符串
 */
QString SqlEditor::textUnderCursor() const
{
    // 获取当前光标位置
    QTextCursor cursor = textCursor();
    int currentPos = cursor.position();
    
    // 从光标位置向前扫描，找到单词的起始位置
    int startPos = currentPos;
    while (startPos > 0) {
        // 移动到前一个字符
        cursor.setPosition(startPos - 1);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QChar ch = cursor.selectedText().at(0);

        // 如果字符不是字母、数字或下划线，说明单词边界已到
        if (!ch.isLetterOrNumber() && ch != '_') {
            break;
        }

        // 继续向前扫描
        startPos--;
    }
    
    // 恢复光标到原始位置
    cursor = textCursor();
    cursor.setPosition(startPos);
    
    // 从 startPos 移动到 currentPos，选中整个单词
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, currentPos - startPos);
    
    // 返回选中的文本（即当前单词）
    return cursor.selectedText().trimmed();
}

/**
 * @brief 插入补全内容到编辑器
 * @details 将选中的补全项替换当前正在输入的单词。
 *          实现逻辑：
 *          1. 先获取当前已输入的单词（如 "inse"）
 *          2. 选中文本中的该单词
 *          3. 用完整的补全内容（如 "INSERT"）替换
 *          使用 QTextCursor 进行文本替换，确保光标位置正确更新。
 *          该函数由 QCompleter 的 activated 信号触发。
 * @author YuzhSong
 * @param completion 要插入的补全字符串
 */
void SqlEditor::insertCompletion(const QString& completion)
{
    // 如果补全器当前不属于本编辑器，则忽略
    if (sqlCompleter->widget() != this) {
        return;
    }

    // 获取当前已输入的单词（如 "inse"）
    QString currentWord = textUnderCursor();
    
    // 如果当前没有单词，则直接插入
    if (currentWord.isEmpty()) {
        textCursor().insertText(completion);
        return;
    }

    // 获取当前光标对象
    QTextCursor cursor = textCursor();
    
    // 向前移动光标，选中当前已输入的单词
    // 例如：光标在 "inse|" 位置，需要选中 "inse"
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, currentWord.length());
    
    // 用完整的补全内容替换已输入的单词
    // 例如：将 "inse" 替换为 "INSERT"
    cursor.insertText(completion + " ");
}

/**
 * @brief 触发自动补全
 * @details 检查当前输入的单词是否满足补全条件（长度 >= 1），
 *          如果满足则弹出补全框并显示候选列表。
 *          使用 completer->complete(cursorRect()) 定位补全框位置，
 *          确保补全框出现在光标附近。
 *          当补全框已可见时，仍然更新前缀以实现实时过滤。
 * @author YuzhSong
 */
void SqlEditor::triggerCompletion()
{
    // 如果补全器为空，则不再触发
    if (!sqlCompleter) {
        return;
    }

    // 获取当前光标位置的单词
    QString currentWord = textUnderCursor();

    // 如果当前没有单词（例如输入了空格、标点符号等），关闭补全框
    if (currentWord.isEmpty()) {
        if (sqlCompleter->popup()->isVisible()) {
            sqlCompleter->popup()->hide();
        }
        return;
    }

    // 至少输入 1 个字符就触发补全
    if (currentWord.length() >= 1) {
        // 设置补全器的前缀为当前单词（实时更新过滤）
        sqlCompleter->setCompletionPrefix(currentWord);

        // 如果补全框已可见，只需要更新前缀即可，QCompleter 会自动刷新列表
        if (sqlCompleter->popup()->isVisible()) {
            // 检查更新后是否还有候选项
            if (sqlCompleter->completionCount() == 0) {
                // 没有匹配项，关闭补全框
                sqlCompleter->popup()->hide();
            }
            return;
        }

        // 如果当前前缀的候选项数量大于 0 且与当前单词不同，则弹出补全框
        QString completion = sqlCompleter->currentCompletion();
        if (sqlCompleter->completionCount() > 0 &&
            completion.toLower() != currentWord.toLower()) {
            QRect cr = cursorRect();
            cr.setWidth(sqlCompleter->completionPrefix().length() * fontMetrics().horizontalAdvance('X') + 20);
            sqlCompleter->complete(cr);
            QTimer::singleShot(0, [this]() {
                if (sqlCompleter && sqlCompleter->popup()->isVisible()) {
                    sqlCompleter->popup()->setCurrentIndex(
                        sqlCompleter->popup()->model()->index(0, 0));
                }
            });
        } else if (sqlCompleter->completionCount() > 1) {
            QRect cr = cursorRect();
            cr.setWidth(fontMetrics().horizontalAdvance(currentWord) + 20);
            sqlCompleter->complete(cr);
            QTimer::singleShot(0, [this]() {
                if (sqlCompleter && sqlCompleter->popup()->isVisible()) {
                    sqlCompleter->popup()->setCurrentIndex(
                        sqlCompleter->popup()->model()->index(0, 0));
                }
            });
        }
    }
}

/**
 * @brief 重置事件处理函数
 * @details 当窗口大小变化时，更新行号区域尺寸
 * @author YuzhSong
 * @param event 重置事件对象
 */
void SqlEditor::resizeEvent(QResizeEvent* event)
{
    // 调用父类默认处理
    QPlainTextEdit::resizeEvent(event);

    // 更新行号区域的几何尺寸
    if (lineNumberArea) {
        QRect cr = contentsRect();
        lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                         lineNumberArea->lineNumberAreaWidth(), cr.height()));
    }
}

/**
 * @brief 按键按下事件处理函数
 * @details 处理 Tab 键、回车的自动缩进逻辑，以及自动补全的键盘交互。
 *          补全交互逻辑：
 *          - 当补全框可见时，Up/Down 键用于在候选项中上下选择
 *          - Enter/Tab 键用于完成补全并插入选中的候选项
 *          - Escape 键用于关闭补全框
 *          - 当补全框不可见时，输入字母/数字时触发补全
 *          - 其他键（如回车）执行默认行为（自动缩进等）
 * @author YuzhSong
 * @param event 按键事件对象
 */
void SqlEditor::keyPressEvent(QKeyEvent* event)
{
    // ========== 自动配对括号与引号 ==========
    const QString text = event->text();
    if (text == QStringLiteral("(")) {
        QTextCursor cursor = textCursor();
        cursor.insertText("()");
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        setTextCursor(cursor);
        event->accept();
        triggerCompletion();
        return;
    }
    if (text == QStringLiteral("[")) {
        QTextCursor cursor = textCursor();
        cursor.insertText("[]");
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        setTextCursor(cursor);
        event->accept();
        triggerCompletion();
        return;
    }
    if (text == QStringLiteral("{")) {
        QTextCursor cursor = textCursor();
        cursor.insertText("{}");
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        setTextCursor(cursor);
        event->accept();
        triggerCompletion();
        return;
    }
    if (text == QStringLiteral("\"")) {
        QTextCursor cursor = textCursor();
        cursor.insertText("\"\"");
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        setTextCursor(cursor);
        event->accept();
        triggerCompletion();
        return;
    }
    if (text == QStringLiteral("'")) {
        QTextCursor cursor = textCursor();
        cursor.insertText("''");
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        setTextCursor(cursor);
        event->accept();
        triggerCompletion();
        return;
    }

    // ========== 自动补全相关逻辑 ==========
    
    // 如果补全框当前可见，优先处理补全相关的键盘交互
    if (sqlCompleter && sqlCompleter->popup()->isVisible()) {
        // 处理以下键：
        // - Enter/Tab：完成补全
        // - Up/Down：在候选项中上下选择
        // - Escape：关闭补全框
        // - PageUp/PageDown：翻页
        // - Home/End：跳到第一个/最后一个
        // - 任何修改文本的键（字母、数字等）：继续输入并更新补全
        
        int key = event->key();
        
        // Enter 或 Tab 键：完成补全（Ctrl+Enter 不拦，留给快捷键执行 SQL）
        if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Tab) {
            if ((key == Qt::Key_Enter || key == Qt::Key_Return)
                && (event->modifiers() & Qt::ControlModifier)) {
                sqlCompleter->popup()->hide();
                QPlainTextEdit::keyPressEvent(event);
                return;
            }
            // 获取当前用户选中的候选项（不是第一项！）
            // 使用 popup()->currentIndex() 获取用户当前高亮的索引
            QModelIndex currentIndex = sqlCompleter->popup()->currentIndex();
            QString completion = sqlCompleter->completionModel()->data(currentIndex).toString();

            // 隐藏补全框
            sqlCompleter->popup()->hide();

            // 插入补全内容
            insertCompletion(completion);

            // 标记事件已处理
            event->accept();
            return;
        }
        
        // Escape 键：关闭补全框
        if (key == Qt::Key_Escape) {
            sqlCompleter->popup()->hide();
            event->accept();
            return;
        }
        
        // Up/Down 键：让补全器自己处理（在候选项中上下选择）
        if (key == Qt::Key_Up || key == Qt::Key_Down || 
            key == Qt::Key_PageUp || key == Qt::Key_PageDown ||
            key == Qt::Key_Home || key == Qt::Key_End) {
            // 调用父类默认处理，让 QCompleter 捕获这些键
            QPlainTextEdit::keyPressEvent(event);
            return;
        }
        
        // 如果是字母、数字或其他字符键：先插入字符，然后触发补全
        if (event->text().isEmpty() || 
            (key == Qt::Key_Backspace || key == Qt::Key_Delete)) {
            // Backspace/Delete：删除字符后继续补全
            QPlainTextEdit::keyPressEvent(event);
            // 延迟触发补全（等待文本更新）
            triggerCompletion();
            return;
        }
        
        // 其他可打印字符：插入字符并触发补全
        QPlainTextEdit::keyPressEvent(event);
        triggerCompletion();
        return;
    }
    
    // ========== 补全框不可见时的默认逻辑 ==========
    
    // 处理回车键：自动缩进（Ctrl+Enter 不拦）
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ControlModifier) {
            QPlainTextEdit::keyPressEvent(event);
            return;
        }
        // 获取当前行的缩进
        QString indentation = getCurrentLineIndentation();

        // 插入换行和缩进
        QTextCursor cursor = textCursor();
        cursor.insertText("\n" + indentation);

        // 标记事件已处理
        event->accept();
        return;
    }

    // 处理 Tab 键：插入空格替代
    if (event->key() == Qt::Key_Tab) {
        // 插入 4 个空格代替 Tab
        QString spaces(4, ' ');
        textCursor().insertText(spaces);
        event->accept();
        return;
    }
    
    // 调用默认处理
    QPlainTextEdit::keyPressEvent(event);
    
    // 输入字母或数字时触发补全
    // 检查按下的键是否是字母或数字（用于触发补全）
    int key = event->key();
    bool isLetterOrNumber = (key >= Qt::Key_A && key <= Qt::Key_Z) ||
                            (key >= Qt::Key_0 && key <= Qt::Key_9) ||
                            key == Qt::Key_Underscore;
    
    if (isLetterOrNumber) {
        // 延迟一点触发补全，确保文本已更新
        triggerCompletion();
    }
}

/**
 * @brief 焦点进入事件处理函数
 * @details 当编辑器获得焦点时，绑定 QCompleter 以启用自动补全。
 *          确保补全器始终与当前编辑器实例关联。
 * @author YuzhSong
 * @param event 焦点事件对象
 */
void SqlEditor::focusInEvent(QFocusEvent* event)
{
    // 如果补全器存在，确保它绑定到当前编辑器
    if (sqlCompleter) {
        sqlCompleter->setWidget(this);
    }
    
    // 调用父类默认处理
    QPlainTextEdit::focusInEvent(event);
}

/**
 * @brief 更新请求处理槽函数
 * @details 处理编辑区的更新请求，同步刷新行号区域
 * @author YuzhSong
 * @param rect 更新区域
 * @param dy 垂直偏移量
 */
void SqlEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    // 如果有垂直滚动偏移，则滚动行号区域
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        // 否则刷新整个行号区域
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    // 如果更新区域包含行号区域，则强制重绘
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

/**
 * @brief 块数量变化处理槽函数
 * @details 当文本块数量变化时，调整行号区域宽度
 * @author YuzhSong
 */
void SqlEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    if (lineNumberVisible && lineNumberArea != nullptr) {
        setViewportMargins(lineNumberArea->lineNumberAreaWidth(), 0, 0, 0);
    } else {
        setViewportMargins(0, 0, 0, 0);
    }
}

/**
 * @brief 光标位置变化处理槽函数
 * @details 光标移动时高亮当前行
 * @author YuzhSong
 */
void SqlEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        const QColor lineColor = ThemeManager::isDark() ? QColor("#1A1F26") : QColor("#E8EDF4");

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }

    // 括号匹配高亮
    QTextCursor cursor = textCursor();
    const QString fullText = toPlainText();
    const int pos = cursor.position();
    if (pos >= 0 && pos <= fullText.length()) {
        // 检查光标前或光标处的括号
        for (int offset = 0; offset <= 1; ++offset) {
            const int checkPos = pos - offset;
            if (checkPos < 0 || checkPos >= fullText.length()) continue;
            const QChar ch = fullText.at(checkPos);
            QChar openChar, closeChar;
            int direction = 0;
            if (ch == '(' || ch == '[' || ch == '{') { openChar = ch; closeChar = (ch == '(') ? ')' : (ch == '[') ? ']' : '}'; direction = 1; }
            else if (ch == ')' || ch == ']' || ch == '}') { closeChar = ch; openChar = (ch == ')') ? '(' : (ch == ']') ? '[' : '{'; direction = -1; }
            if (direction == 0) continue;

            int depth = 0;
            int matchPos = -1;
            for (int i = checkPos; i >= 0 && i < fullText.length(); i += direction) {
                if (fullText.at(i) == openChar) ++depth;
                else if (fullText.at(i) == closeChar) --depth;
                if (depth == 0) { matchPos = i; break; }
            }
            if (matchPos >= 0) {
                QTextEdit::ExtraSelection sel;
                sel.format.setBackground(QColor(ThemeManager::isDark() ? "#4A5568" : "#BBD3F5"));
                sel.cursor = QTextCursor(document());
                sel.cursor.setPosition(checkPos);
                sel.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                extraSelections.append(sel);
                sel.cursor = QTextCursor(document());
                sel.cursor.setPosition(matchPos);
                sel.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                extraSelections.append(sel);
            }
            break;
        }
    }

    setExtraSelections(extraSelections);
    lineNumberArea->update();
}

/**
 * @brief 滚动条值变化处理槽函数
 * @details 滚动编辑器时同步滚动行号区域
 * @author YuzhSong
 * @param value 滚动条值
 */
void SqlEditor::onVerticalScrollBarValueChanged(int value)
{
    // 行号区域会随着 updateRequest 自动滚动
    // 此处确保行号刷新
    lineNumberArea->update();
}

/**
 * @brief 获取行号区域宽度并调整编辑区边距
 * @details 由 LineNumberArea 调用，用于同步更新布局
 * @author YuzhSong
 */
void SqlEditor::lineNumberAreaWidthUpdate()
{
    updateLineNumberAreaWidth(0);
}

/**
 * @brief 绘制行号区域
 * @details 由 LineNumberArea::paintEvent 调用，负责实际绘制行号。
 *          因为需要访问 QPlainTextEdit 的 protected 成员函数，
 *          所以绘制逻辑必须放在 SqlEditor 中。
 * @author YuzhSong
 * @param event 绘制事件对象
 */
void SqlEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    if (!lineNumberVisible) {
        return;
    }

    QPainter painter(lineNumberArea);

    const bool dark = ThemeManager::isDark();
    const QColor bgColor = dark ? QColor("#5A5A5A") : QColor("#E8ECF0");
    const QColor defaultFg = dark ? QColor("#DADADA") : QColor("#8A92A0");
    const QColor currentFg = dark ? QColor("#FFFFFF") : QColor("#3B71CA");

    painter.fillRect(event->rect(), bgColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top + blockBoundingRect(block).height();

    int currentLineNumber = textCursor().blockNumber();

    QFont font = painter.font();
    font.setBold(false);
    painter.setFont(font);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if (blockNumber == currentLineNumber) {
                painter.setPen(currentFg);
                QFont boldFont = painter.font();
                boldFont.setBold(true);
                painter.setFont(boldFont);
            } else {
                painter.setPen(defaultFg);
            }

            painter.drawText(0, qRound(top),
                           lineNumberArea->width() - 4, fontMetrics().height(),
                           Qt::AlignRight | Qt::AlignVCenter,
                           number);

            if (blockNumber == currentLineNumber) {
                QFont normalFont = painter.font();
                normalFont.setBold(false);
                painter.setFont(normalFont);
            }
        }

        block = block.next();
        top = bottom;
        if (block.isValid()) {
            bottom = top + blockBoundingRect(block).height();
        }
        ++blockNumber;
    }
}

/**
 * @brief 调整编辑器滚动区域边距
 * @details 为行号区域留出空间
 * @author YuzhSong
 */
void SqlEditor::updateEditorViewportMargins()
{
    updateLineNumberAreaWidth(0);
}

void SqlEditor::setAutoWrapEnabled(bool enabled)
{
    setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void SqlEditor::setLineNumberVisible(bool enabled)
{
    lineNumberVisible = enabled;
    if (lineNumberArea != nullptr) {
        lineNumberArea->setVisible(enabled);
    }
    updateLineNumberAreaWidth(0);
    viewport()->update();
}

void SqlEditor::setCompletionNames(const QStringList &names)
{
    if (!sqlCompleter || names.isEmpty()) {
        return;
    }

    QStringList allWords = baseKeywords;
    allWords.append(names);
    allWords.removeDuplicates();

    QAbstractItemModel *oldModel = sqlCompleter->model();
    sqlCompleter->setModel(new QStringListModel(allWords, sqlCompleter));
    if (oldModel != nullptr) {
        oldModel->deleteLater();
    }
}

void SqlEditor::refreshTheme()
{
    setStyleSheet(ThemeManager::sqlEditor());

    const bool dark = ThemeManager::isDark();
    viewport()->setAutoFillBackground(true);
    QPalette vpPal = viewport()->palette();
    vpPal.setColor(QPalette::Base, dark ? QColor("#111315") : QColor("#FFFFFF"));
    vpPal.setColor(QPalette::Text, dark ? QColor("#E6E6E6") : QColor("#1C1E21"));
    viewport()->setPalette(vpPal);

    if (lineNumberArea != nullptr) {
        lineNumberArea->setAutoFillBackground(true);
        QPalette lnPal = lineNumberArea->palette();
        lnPal.setColor(QPalette::Window, dark ? QColor("#5A5A5A") : QColor("#E8ECF0"));
        lineNumberArea->setPalette(lnPal);
    }

    if (sqlCompleter != nullptr && sqlCompleter->popup() != nullptr) {
        QPalette popPal = sqlCompleter->popup()->palette();
        popPal.setColor(QPalette::Base, dark ? QColor("#1E1F22") : QColor("#FFFFFF"));
        popPal.setColor(QPalette::Text, dark ? QColor("#D7DAE0") : QColor("#1C1E21"));
        sqlCompleter->popup()->setPalette(popPal);
    }

    highlightCurrentLine();
    lineNumberArea->update();
}
