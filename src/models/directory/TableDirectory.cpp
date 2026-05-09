#include "TableDirectory.h"

#include <nlohmann/json.hpp>

std::string TableDirectory::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["belongDataBase"] = belongDataBase;
    jsonObject["tables"] = tables;
    return jsonObject.dump();
}

TableDirectory TableDirectory::fromJson(const std::string &jsonStr)
{
    const nlohmann::json jsonObject = nlohmann::json::parse(jsonStr);

    TableDirectory directory;
    directory.setBelongDataBase(jsonObject.value("belongDataBase", ""));
    directory.setTables(jsonObject.value("tables", std::set<std::string>{}));
    return directory;
}
