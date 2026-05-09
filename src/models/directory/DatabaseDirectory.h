#pragma once

#include <set>
#include <string>

/**
 * @class DatabaseDirectory
 * @brief 数据库目录数据类
 * @details 维护当前可见的数据库名称集合。
 * @author NAPH130
 */
class DatabaseDirectory
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
     * @return 反序列化后的数据库目录对象
     */
    static DatabaseDirectory fromJson(const std::string &jsonStr);

    /**
     * @brief 获取数据库集合
     * @author NAPH130
     * @return 数据库名称集合
     */
    const std::set<std::string> &getDatabases() const
    {
        return databases;
    }

    /**
     * @brief 设置数据库集合
     * @author NAPH130
     * @param databaseSet 数据库名称集合
     */
    void setDatabases(const std::set<std::string> &databaseSet)
    {
        databases = databaseSet;
    }

private:
    std::set<std::string> databases;
};
