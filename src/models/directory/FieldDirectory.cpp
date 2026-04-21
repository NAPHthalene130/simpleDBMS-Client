#include "FieldDirectory.h"

#include <nlohmann/json.hpp>

std::string FieldDirectory::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["belongTable"] = belongTable;
    jsonObject["fields"] = fields;
    return jsonObject.dump();
}

FieldDirectory FieldDirectory::fromJson(const std::string &jsonStr)
{
    const nlohmann::json jsonObject = nlohmann::json::parse(jsonStr);

    FieldDirectory directory;
    directory.setBelongTable(jsonObject.value("belongTable", ""));
    directory.setFields(jsonObject.value("fields", std::set<std::string>{}));
    return directory;
}
