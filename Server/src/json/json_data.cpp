//
// Created by eow on 30/06/2026.
//

#include "json_data.h"


namespace json_data
{
    JsonString::JsonString(std::string v)
    : value_(std::move(v))
    {}

    JsonNumber::JsonNumber(double v)
    : value_(v)
    {}
    JsonBool::JsonBool(bool v)
    : value_(v)
    {}

    void JsonObject::setMember(std::string key, JsonValuePtr value)
    {
        members_[std::move(key)] = std::move(value);
    }

    const JsonValue* JsonObject::find(const std::string& key) const
    {
        auto it = members_.find(key);
        return (it != members_.end()) ? it->second.get() : nullptr;
    }

    JsonValue* JsonObject::findMutable(const std::string& key)
    {
        auto it = members_.find(key);
        return (it != members_.end()) ? it->second.get() : nullptr;
    }

// --- Array ---
    void JsonArray::pushBack(JsonValuePtr item)
    {
        items_.push_back(std::move(item));
    }

    const JsonValue* JsonArray::at(size_t index) const
    {
        if (index >= items_.size()) return nullptr;
        return items_[index].get();
    }

    JsonValue* JsonArray::atMutable(size_t index)
    {
        if (index >= items_.size()) return nullptr;
        return items_[index].get();
    }

}


