#pragma once

#include <set>
#include <string>

/**
 * @class FieldDirectory
 * @brief 字段目录数据类
 * @details 记录所属数据表以及该数据表下的字段集合。
 * @author NAPH130
 */
class FieldDirectory
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
     * @return 反序列化后的字段目录对象
     */
    static FieldDirectory fromJson(const std::string &jsonStr);

    /**
     * @brief 获取所属数据表名称
     * @author NAPH130
     * @return 所属数据表名称
     */
    const std::string &getBelongTable() const
    {
        return belongTable;
    }

    /**
     * @brief 设置所属数据表名称
     * @author NAPH130
     * @param tableName 所属数据表名称
     */
    void setBelongTable(const std::string &tableName)
    {
        belongTable = tableName;
    }

    /**
     * @brief 获取字段集合
     * @author NAPH130
     * @return 字段名称集合
     */
    const std::set<std::string> &getFields() const
    {
        return fields;
    }

    /**
     * @brief 设置字段集合
     * @author NAPH130
     * @param fieldSet 字段名称集合
     */
    void setFields(const std::set<std::string> &fieldSet)
    {
        fields = fieldSet;
    }

private:
    std::string belongTable;
    std::set<std::string> fields;
};
