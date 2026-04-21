#include "DatabaseDirectory.h"

#include <nlohmann/json.hpp>

std::string DatabaseDirectory::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["databases"] = databases;
    return jsonObject.dump();
}

DatabaseDirectory DatabaseDirectory::fromJson(const std::string &jsonStr)
{
    const nlohmann::json jsonObject = nlohmann::json::parse(jsonStr);

    DatabaseDirectory directory;
    directory.setDatabases(jsonObject.value("databases", std::set<std::string>{}));
    return directory;
}
