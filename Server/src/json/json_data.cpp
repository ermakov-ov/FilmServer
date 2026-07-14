#include <sstream>
#include "json_data.h"

namespace json_data
{
// --- String ---
    JsonString::JsonString(std::string v)
    : value_(std::move(v))
    {}

    std::string JsonString::toString() const
    {
        std::string out;

        out.reserve(value_.size() + 4);

        out.push_back('"');
        for (char c : value_) {
            switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n";  break;
                case '\r': out += "\\r";  break;
                case '\t': out += "\\t";  break;
                default:   out.push_back(c); break;
            }
        }
        out.push_back('"');

        return out;
    }

// --- Number ---
    JsonNumber::JsonNumber(double v)
    : value_(v)
    {}

    std::string JsonNumber::toString() const
    {
        std::ostringstream oss;
        oss << value_;
        return oss.str();
    }

// --- Bool ---
    JsonBool::JsonBool(bool v)
    : value_(v)
    {}
    std::string JsonBool::toString() const
    {
        return value_ ? "true" : "false";
    }

// --- Object ---
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

    std::string JsonObject::toString() const
    {
        if (members_.empty()) {
            return "{}";
        }

        std::string result;
        result.reserve(128); // стартовый буфер, дальше сам вырастет
        result += "{";

        bool first = true;
        for (const auto& [key, valPtr] : members_) {
            if (!first) {
                result += ',';
            }
            first = false;

            result += JsonString(key).toString(); // ключ экранируется как строка
            result += ':';
            result += valPtr->toString();         // рекурсия для значения
        }

        result += '}';
        return result;
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
    std::string JsonArray::toString() const
    {
        if (items_.empty()) {
            return "[]";
        }

        std::string result;
        result.reserve(64);
        result += '[';

        bool first = true;
        for (const auto& itemPtr : items_) {
            if (!first) {
                result += ',';
            }
            first = false;
            result += itemPtr->toString(); // рекурсия
        }

        result += ']';
        return result;
    }

    std::string JsonNull::toString() const
    {
        return "null";
    }

}


