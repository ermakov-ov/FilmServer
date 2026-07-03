#ifndef SERVER_JSON_DATA_H
#define SERVER_JSON_DATA_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "../common/common.h"

namespace json_data
{
    class JsonValue ;
    using JsonValuePtr      = std::unique_ptr<JsonValue>;
    using JsonValueMapData   = std::unordered_map<std::string, JsonValuePtr>;
    using JsonValueArrayData = std::vector<JsonValuePtr>;

    class JsonValue {
    public:
        enum class Type { Null, String, Number, Bool, Object, Array };

        virtual ~JsonValue() = default;
        virtual Type type() const = 0;

        virtual const std::string& asString() const { throw std::runtime_error("Type mismatch: expected string");};
        virtual double asNumber() const { throw std::runtime_error("Type mismatch: expected number"); };
        virtual bool asBool() const { throw std::runtime_error("Type mismatch: expected bool"); };

        // Для объектов
        virtual const JsonValueMapData& asObject() const { throw std::runtime_error("Type mismatch: expected object");};
        virtual JsonValueMapData& asObjectMutable() { throw std::runtime_error("Type mismatch: expected object"); };

        // Для массивов
        virtual const JsonValueArrayData& asArray() const { throw std::runtime_error("Type mismatch: expected array");};
        virtual JsonValueArrayData& asArrayMutable() { throw std::runtime_error("Type mismatch: expected array");} ;

        bool isNull() const { return type() == Type::Null; }
        bool isString() const { return type() == Type::String; }
        bool isNumber() const { return type() == Type::Number; }
        bool isBool() const { return type() == Type::Bool; }
        bool isObject() const { return type() == Type::Object; }
        bool isArray() const { return type() == Type::Array; }
    };

// --- Null ---
    class JsonNull : public JsonValue {
    public:
        Type type() const override { return Type::Null; };
    };

// --- String ---
    class JsonString : public JsonValue {
        std::string value_;
    public:
        explicit JsonString(std::string v) ;
        Type type() const override { return Type::String; };
        const std::string& asString() const override { return value_; };
    };

// --- Number (double) ---
    class JsonNumber : public JsonValue {
        double value_;
    public:
        explicit JsonNumber(double v) ;
        Type type() const override { return Type::Number; };
        double asNumber() const override { return value_; };
    };

// --- Bool ---
    class JsonBool : public JsonValue {
        bool value_;
    public:
        explicit JsonBool(bool v) ;
        Type type() const override { return Type::Bool; };
        bool asBool() const override { return value_; };
    };

// --- Object (key:value) ---
    class JsonObject : public JsonValue {
        JsonValueMapData members_;
    public:
        Type type() const override { return Type::Object; }
        void setMember(std::string key, JsonValuePtr value) ;
        const JsonValue* find(const std::string& key) const ;
        JsonValue* findMutable(const std::string& key) ;
        const JsonValueMapData& asObject() const override { return members_; };
        JsonValueMapData& asObjectMutable() override { return members_; };
    };

// --- Array ---
    class JsonArray : public JsonValue {
        JsonValueArrayData items_;
    public:
        Type type() const override { return Type::Array; }
        void pushBack(JsonValuePtr item) ;
        size_t size() const { return items_.size(); }
        const JsonValue* at(size_t index) const ;
        JsonValue* atMutable(size_t index) ;
        const JsonValueArrayData& asArray() const override { return items_; }
        JsonValueArrayData& asArrayMutable() override { return items_; }
    };

// ---
    struct JsonFactory {
        static JsonValuePtr makeNull() {
            return std::make_unique<JsonNull>();
        }
        static JsonValuePtr makeString(std::string s) {
            return std::make_unique<JsonString>(std::move(s));
        }
        static JsonValuePtr makeNumber(double n) {
            return std::make_unique<JsonNumber>(n);
        }
        static JsonValuePtr makeBool(bool b) {
            return std::make_unique<JsonBool>(b);
        }
        static JsonValuePtr makeObject() {
            return std::make_unique<JsonObject>();
        }
        static JsonValuePtr makeArray() {
            return std::make_unique<JsonArray>();
        }
    };
}

#endif //SERVER_JSON_DATA_H
