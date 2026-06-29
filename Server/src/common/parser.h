//
// Created by eow on 23/06/2026.
//

#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "const.h"

namespace parser
{
    enum class LexemReadStatus
    {
        LRS_NO_ERROR=0,
        LRS_ERROR=1

    };

    using DataPtr = std::unique_ptr<char[]>;

    class BufferData
    {
    public:
        BufferData(DataPtr &ex_data, std::size_t length, std::size_t offset=0);
        ~BufferData();
        LexemReadStatus nextPosition() ;
        parser_const::LexemFirstType getStartChar() const ;

        std::string get_lexem() ;
    private:


        LexemReadStatus get_lexem(std::string &ret_value) ;
        bool isDoubleQuotes(const char ch) const ;
        bool isSingleQuotes(const char ch) const ;
        bool isColon(const char ch) const ;
        bool isOpenBracket(const char ch) const ;
        bool isCloseBracket(const char ch) const ;
        bool isComma(const char ch) const ;
        bool isDigit(const char ch) const ;
        bool isChar(const char ch) const ;
        bool isCharDigit(const char ch) const ;
        
        
        
        DataPtr     m_data;
        std::size_t m_length;
        std::size_t m_offset;
    };
    class JsonValue ;
    using JsonValuePtr      = std::unique_ptr<JsonValue>;
    using JsonValueMapData   = std::unordered_map<std::string, JsonValuePtr>;
    using JsonValueArrayData = std::vector<JsonValuePtr>;
    
// --- Базовый класс для любого JSON-значения ---
    class JsonValue {
    public:
        enum class Type { Null, String, Number, Bool, Object, Array };

        virtual ~JsonValue() = default;
        virtual Type type() const = 0;

        // Приведение к конкретным типам (throw если тип не совпадает)
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
        Type type() const override { return Type::Null; }
    };

// --- String ---
    class JsonString : public JsonValue {
        std::string value_;
    public:
        explicit JsonString(std::string v) : value_(std::move(v)) {}
        Type type() const override { return Type::String; }
        const std::string& asString() const override { return value_; }
    };

// --- Number (double) ---
    class JsonNumber : public JsonValue {
        double value_;
    public:
        explicit JsonNumber(double v) : value_(v) {}
        Type type() const override { return Type::Number; }
        double asNumber() const override { return value_; }
    };

// --- Bool ---
    class JsonBool : public JsonValue {
        bool value_;
    public:
        explicit JsonBool(bool v) : value_(v) {}
        Type type() const override { return Type::Bool; }
        bool asBool() const override { return value_; }
    };

// --- Object (key:value) ---
    class JsonObject : public JsonValue {
        JsonValueMapData members_;
    public:
        Type type() const override { return Type::Object; }

        void setMember(std::string key, JsonValuePtr value) {
            members_[std::move(key)] = std::move(value);
        }

        const JsonValue* find(const std::string& key) const {
            auto it = members_.find(key);
            return (it != members_.end()) ? it->second.get() : nullptr;
        }

        JsonValue* findMutable(const std::string& key) {
            auto it = members_.find(key);
            return (it != members_.end()) ? it->second.get() : nullptr;
        }

        const JsonValueMapData& asObject() const override { return members_; }
        JsonValueMapData& asObjectMutable() override { return members_; }
    };

// --- Array ---
    class JsonArray : public JsonValue {
        JsonValueArrayData items_;
    public:
        Type type() const override { return Type::Array; }

        void pushBack(JsonValuePtr item) {
            items_.push_back(std::move(item));
        }

        size_t size() const { return items_.size(); }

        const JsonValue* at(size_t index) const {
            if (index >= items_.size()) return nullptr;
            return items_[index].get();
        }

        JsonValue* atMutable(size_t index) {
            if (index >= items_.size()) return nullptr;
            return items_[index].get();
        }

        const JsonValueArrayData& asArray() const override { return items_; }
        JsonValueArrayData& asArrayMutable() override { return items_; }
    };

// --- Фабрика для удобного создания ---
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
#endif //SERVER_PARSER_H
