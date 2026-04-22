/**
 * @file sqlHighlighter.h
 * @brief SQL 语法高亮器头文件
 * @details 继承 QSyntaxHighlighter，为 SQL 编辑器提供语法高亮功能。
 *          支持 SQL 关键字、字符串、单行注释和多行注释的高亮。
 * @author YuzhSong
 * @date 2026-04-11
 */

#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

/**
 * @class SqlHighlighter
 * @brief SQL 语法高亮器类
 * @details 继承自 QSyntaxHighlighter，提供以下高亮规则：
 *          - SQL 关键字（SELECT, FROM, WHERE 等）：蓝色加粗
 *          - 字符串（'xxx'）：绿色
 *          - 单行注释（-- comment）：灰色斜体
 *          - 多行注释（/*）：灰色斜体
 *          所有高亮规则在类中通过结构体清晰定义，便于扩展和维护。
 * @author YuzhSong
 */
class SqlHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @author YuzhSong
     * @param parent 父对象指针
     * @param document 关联的文本文档
     */
    explicit SqlHighlighter(QObject* parent = nullptr, QTextDocument* document = nullptr);

    /**
     * @brief 析构函数
     * @author YuzhSong
     */
    ~SqlHighlighter();

protected:
    /**
     * @brief 高亮处理函数
     * @details 重写基类方法，对每一行文本应用所有高亮规则
     * @author YuzhSong
     * @param text 当前行文本内容
     */
    void highlightBlock(const QString& text) override;

private:
    /**
     * @brief 高亮规则结构体
     * @details 存储单个高亮模式的配置信息
     */
    struct HighlightingRule {
        QRegularExpression pattern;   ///< 正则表达式模式（Qt6）
        QTextCharFormat format;       ///< 字符格式（颜色、字体等）
    };

    /**
     * @brief 初始化所有高亮规则
     * @details 配置关键字、字符串、注释等的高亮样式
     * @author YuzhSong
     */
    void setupHighlightingRules();

    /**
     * @brief 高亮 SQL 关键字
     * @details 匹配 SELECT, FROM, WHERE 等常用 SQL 关键字
     * @author YuzhSong
     * @param text 当前行文本
     */
    void highlightKeywords(const QString& text);

    /**
     * @brief 高亮字符串常量
     * @details 匹配单引号包围的字符串 'xxx'
     * @author YuzhSong
     * @param text 当前行文本
     */
    void highlightStrings(const QString& text);

    /**
     * @brief 高亮单行注释
     * @details 匹配 -- 开头的注释内容
     * @author YuzhSong
     * @param text 当前行文本
     */
    void highlightSingleLineComments(const QString& text);

    /**
     * @brief 高亮多行注释
     * @details 匹配 /* ...包围的注释内容
     * @author YuzhSong
     * @param text 当前行文本
     */
    void highlightMultiLineComments(const QString& text);

    QVector<HighlightingRule> highlightingRules;  ///< 高亮规则列表
    QTextCharFormat keywordFormat;                ///< 关键字格式
    QTextCharFormat stringFormat;                 ///< 字符串格式
    QTextCharFormat singleLineCommentFormat;      ///< 单行注释格式
    QTextCharFormat multiLineCommentFormat;       ///< 多行注释格式
};
