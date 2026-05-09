#pragma once

#include <set>
#include <string>

/**
 * @class TableDirectory
 * @brief 数据表目录数据类
 * @details 记录所属数据库以及该数据库下的数据表集合。
 * @author NAPH130
 */
class TableDirectory
{
public:
    /**
     * @brief 将当前对象序列化为 JSON 字符串
     * @author NAPH130
     * @return JSON 格式字符串
     */
    std::string toJson() const;

    /**
     * @brief 从 JSON 字符串反序列化对象
     * @author NAPH130
     * @param jsonStr JSON 格式字符串
     * @return 反序列化后的数据表目录对象
     */
    static TableDirectory fromJson(const std::string &jsonStr);

    /**
     * @brief 获取所属数据库名称
     * @author NAPH130
     * @return 所属数据库名称
     */
    const std::string &getBelongDataBase() const
    {
        return belongDataBase;
    }

    /**
     * @brief 设置所属数据库名称
     * @author NAPH130
     * @param databaseName 所属数据库名称
     */
    void setBelongDataBase(const std::string &databaseName)
    {
        belongDataBase = databaseName;
    }

    /**
     * @brief 获取数据表集合
     * @author NAPH130
     * @return 数据表名称集合
     */
    const std::set<std::string> &getTables() const
    {
        return tables;
    }

    /**
     * @brief 设置数据表集合
     * @author NAPH130
     * @param tableSet 数据表名称集合
     */
    void setTables(const std::set<std::string> &tableSet)
    {
        tables = tableSet;
    }

private:
    std::string belongDataBase;
    std::set<std::string> tables;
};
