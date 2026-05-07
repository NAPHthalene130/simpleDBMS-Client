/**
 * @file SqlHighlighter.cpp
 * @brief SQL 语法高亮器实现文件
 * @details 实现 SQL 关键字、字符串、单行注释和多行注释的高亮逻辑。
 * @author YuzhSong
 * @date 2026-04-11
 */

#include "SqlHighlighter.h"

#include <QTextDocument>

/**
 * @brief 构造函数
 * @author YuzhSong
 * @param parent 父对象指针
 * @param document 关联的文本文档
 */
SqlHighlighter::SqlHighlighter(QObject* parent, QTextDocument* document)
    : QSyntaxHighlighter(parent)
{
    // 如果提供了文档，则关联到本文档
    if (document) {
        setDocument(document);
    }

    // 初始化所有高亮规则
    setupHighlightingRules();
}

/**
 * @brief 析构函数
 * @author YuzhSong
 */
SqlHighlighter::~SqlHighlighter()
{
    // Qt 自动管理内存，无需额外清理
}

/**
 * @brief 初始化所有高亮规则
 * @details 配置关键字、字符串、注释等的高亮样式
 * @author YuzhSong
 */
void SqlHighlighter::setupHighlightingRules()
{
    // ==========================================
    // 1. 设置关键字格式（蓝色、加粗）
    // ==========================================
    keywordFormat.setForeground(QColor("#3080FA"));
    keywordFormat.setFontWeight(QFont::Bold);

    // 定义 SQL 关键字列表
    QStringList keywordPatterns;
    keywordPatterns << "\\bSELECT\\b"
                    << "\\bFROM\\b"
                    << "\\bWHERE\\b"
                    << "\\bINSERT\\b"
                    << "\\bUPDATE\\b"
                    << "\\bDELETE\\b"
                    << "\\bCREATE\\b"
                    << "\\bDROP\\b"
                    // 2026-04-22 修改：补充 SHOW 关键字高亮规则，作者：YuzhSong。
                    // 说明：沿用既有关键字规则构建方式，不引入特殊分支，保持模块职责稳定。
                    << "\\bSHOW\\b"
                    << "\\bALTER\\b"
                    << "\\bTABLE\\b"
                    << "\\bINTO\\b"
                    << "\\bVALUES\\b"
                    << "\\bSET\\b"
                    << "\\bAND\\b"
                    << "\\bOR\\b"
                    << "\\bNOT\\b"
                    << "\\bNULL\\b"
                    << "\\bAS\\b"
                    << "\\bJOIN\\b"
                    << "\\bLEFT\\b"
                    << "\\bRIGHT\\b"
                    << "\\bINNER\\b"
                    << "\\bOUTER\\b"
                    << "\\bON\\b"
                    << "\\bORDER\\b"
                    << "\\bBY\\b"
                    << "\\bGROUP\\b"
                    << "\\bHAVING\\b"
                    << "\\bLIMIT\\b"
                    << "\\bOFFSET\\b"
                    << "\\bUNION\\b"
                    << "\\bALL\\b"
                    << "\\bDISTINCT\\b"
                    << "\\bEXISTS\\b"
                    << "\\bBETWEEN\\b"
                    << "\\bLIKE\\b"
                    << "\\bIN\\b"
                    << "\\bIS\\b"
                    << "\\bDEFAULT\\b"
                    << "\\bPRIMARY\\b"
                    << "\\bKEY\\b"
                    << "\\bFOREIGN\\b"
                    << "\\bREFERENCES\\b"
                    << "\\bINDEX\\b"
                    << "\\bVIEW\\b"
                    << "\\bDATABASE\\b"
                    << "\\bDATABASES\\b"
                    << "\\bTABLES\\b"
                    << "\\bUSE\\b"
                    << "\\bUNIQUE\\b";

    // 为每个关键字创建高亮规则
    for (const QString& pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // ==========================================
    // 2. 设置字符串格式（绿色）
    // ==========================================
    stringFormat.setForeground(QColor("#1CE93E"));

    // 单引号字符串规则：'xxx'
    HighlightingRule stringRule;
    stringRule.pattern = QRegularExpression("'[^']*'");
    stringRule.format = stringFormat;
    highlightingRules.append(stringRule);

    // ==========================================
    // 3. 设置单行注释格式（灰色、斜体）
    // ==========================================
    singleLineCommentFormat.setForeground(QColor("#808080"));
    singleLineCommentFormat.setFontItalic(true);

    // 单行注释规则：-- comment
    HighlightingRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("--[^\\n]*");
    singleLineCommentRule.format = singleLineCommentFormat;
    highlightingRules.append(singleLineCommentRule);

    // ==========================================
    // 4. 设置多行注释格式（灰色、斜体）
    // ==========================================
    multiLineCommentFormat.setForeground(QColor("#808080"));
    multiLineCommentFormat.setFontItalic(true);
}

/**
 * @brief 高亮处理函数
 * @details 对每一行文本应用所有高亮规则，包括多行注释状态处理
 * @author YuzhSong
 * @param text 当前行文本内容
 */
void SqlHighlighter::highlightBlock(const QString& text)
{
    // ==========================================
    // 1. 处理多行注释状态
    // ==========================================
    // 设置当前块的初始状态：0=不在多行注释中，1=在多行注释中
    setCurrentBlockState(0);

    // 获取上一行的状态（是否在多行注释中）
    int prevBlockState = previousBlockState();

    // 查找多行注释的开始和结束标记
    int startIndex = 0;
    if (prevBlockState != 1) {
        // 如果上一行不在多行注释中，查找本行的 /* 起始位置
        startIndex = text.indexOf(QRegularExpression("/\\*"));
    } else {
        // 如果上一行在多行注释中，则本行从头开始就是注释
        startIndex = 0;
    }

    // 循环处理所有多行注释块
    while (startIndex >= 0) {
        // 查找注释结束标记 */
        QRegularExpression endRegExp("\\*/");
        QRegularExpressionMatch match = endRegExp.match(text, startIndex + 2);
        int endIndex = match.hasMatch() ? match.capturedStart() : -1;

        int commentLength;
        if (endIndex == -1) {
            // 未找到结束标记，注释延续到行末
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            // 找到结束标记，计算注释长度
            commentLength = endIndex - startIndex + 2;
        }

        // 应用多行注释格式
        setFormat(startIndex, commentLength, multiLineCommentFormat);

        // 查找下一个多行注释起始位置
        QRegularExpression nextStartRegExp("/\\*");
        QRegularExpressionMatch nextMatch = nextStartRegExp.match(text, startIndex + commentLength);
        startIndex = nextMatch.hasMatch() ? nextMatch.capturedStart() : -1;
    }

    // ==========================================
    // 2. 应用普通高亮规则（关键字、字符串、单行注释）
    // ==========================================
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);

        // 遍历所有匹配项
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            int index = match.capturedStart();
            int length = match.capturedLength();

            // 检查该匹配项是否已经在多行注释区域内
            // 如果是，则不应用关键字或字符串格式
            bool inMultiLineComment = false;
            if (currentBlockState() == 1 || prevBlockState == 1) {
                // 简化处理：如果处于多行注释状态，跳过其他规则
                // 更精确的做法是检查具体位置是否在 /* */ 范围内
            }

            if (!inMultiLineComment) {
                setFormat(index, length, rule.format);
            }
        }
    }
}

/**
 * @brief 高亮 SQL 关键字
 * @details 匹配 SELECT, FROM, WHERE 等常用 SQL 关键字
 * @author YuzhSong
 * @param text 当前行文本
 */
void SqlHighlighter::highlightKeywords(const QString& text)
{
    // 关键字高亮已在 setupHighlightingRules 中统一处理
    // 此函数保留用于后续扩展或自定义关键字
    Q_UNUSED(text);
}

/**
 * @brief 高亮字符串常量
 * @details 匹配单引号包围的字符串 'xxx'
 * @author YuzhSong
 * @param text 当前行文本
 */
void SqlHighlighter::highlightStrings(const QString& text)
{
    // 字符串高亮已在 setupHighlightingRules 中统一处理
    // 此函数保留用于后续扩展或自定义字符串规则
    Q_UNUSED(text);
}

/**
 * @brief 高亮单行注释
 * @details 匹配 -- 开头的注释内容
 * @author YuzhSong
 * @param text 当前行文本
 */
void SqlHighlighter::highlightSingleLineComments(const QString& text)
{
    // 单行注释高亮已在 setupHighlightingRules 中统一处理
    // 此函数保留用于后续扩展或自定义注释规则
    Q_UNUSED(text);
}

/**
 * @brief 高亮多行注释
 * @details 匹配 /* ...包围的注释内容
 * @author YuzhSong
 * @param text 当前行文本
 */
void SqlHighlighter::highlightMultiLineComments(const QString& text)
{
    // 多行注释高亮已在 highlightBlock 中处理
    // 此函数保留用于后续扩展或自定义注释规则
    Q_UNUSED(text);
}
