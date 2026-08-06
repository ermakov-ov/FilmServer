#include <gtest/gtest.h>
#include <cstring>
#include "../src/json/json_data.h"
#include "../src/json/parser_json.h"
#include "../src/json/stream.h"

static json_data::JsonValuePtr parseJson(const std::string& input)
{
    auto ptr = std::make_unique<char[]>(input.size() + 1);
    std::memcpy(ptr.get(), input.c_str(), input.size());
    ptr[input.size()] = '\0';
    parser::StreamBuffer stream(std::move(ptr), input.size());

    parser::ParserJson parser(std::move(stream));

    return parser.parse();
}

TEST(Parser, ReadsBooleanTrueFromSimpleObject)
{
    const std::string input = R"rawtext({"key1":true,"key2":false,"key3":null,"key4":42,"key5":3.14,"key6":[1,2,3],"key7":{"nested":true}})rawtext";

    auto root = parseJson(input);
    ASSERT_NE(root, nullptr);

    auto* obj = dynamic_cast<json_data::JsonObject*>(root.get());
    ASSERT_NE(obj, nullptr);

    const auto* val = obj->find("key1");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(val->type(), json_data::JsonValue::Type::Bool);
    EXPECT_TRUE(val->asBool());
}

TEST(Parser, ReadsExponentialNumberCorrectly)
{
    const std::string input = R"rawtext({"key1":true,"key2":false,"key3":null,"key4":42,"key5":3.14,"key6":1.23e-10,"key7":[1,2,3],"key8":{"nested":true}})rawtext";

    auto root = parseJson(input);
    ASSERT_NE(root, nullptr);

    auto* obj = dynamic_cast<json_data::JsonObject*>(root.get());
    ASSERT_NE(obj, nullptr);

    const auto* val = obj->find("key6");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(val->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(val->asNumber(), 1.23e-10, 1e-15);
}

TEST(Parser, ReadsNestedValuesFromFormattedJson)
{
    const std::string input = R"rawtext(
    {
      "isActive": true,
      "score": 98.5,
      "multiplier": 2.5e+3,
      "tags": ["red", "green", null],
      "meta": {
        "enabled": false,
        "count": 0
      }
    }
    )rawtext";

    auto root = parseJson(input);
    ASSERT_NE(root, nullptr);

    auto* obj = dynamic_cast<json_data::JsonObject*>(root.get());
    ASSERT_NE(obj, nullptr);

    const auto* metaVal = obj->find("meta");
    ASSERT_NE(metaVal, nullptr);
    const auto* metaObj = dynamic_cast<const json_data::JsonObject*>(metaVal);
    ASSERT_NE(metaObj, nullptr);

    const auto* enabledVal = metaObj->find("enabled");
    ASSERT_NE(enabledVal, nullptr);
    EXPECT_EQ(enabledVal->type(), json_data::JsonValue::Type::Bool);
    EXPECT_FALSE(enabledVal->asBool());

    const auto* countVal = metaObj->find("count");
    ASSERT_NE(countVal, nullptr);
    EXPECT_EQ(countVal->type(), json_data::JsonValue::Type::Number);
    EXPECT_DOUBLE_EQ(countVal->asNumber(), 0.0);
}

TEST(Parser, ReadsValuesFromArrayWithNegativesAndExponents)
{
    const std::string input = R"rawtext([true,false,null,0,-1,3.14,-0.5,1e10,-2.5e-3])rawtext";

    auto root = parseJson(input);
    ASSERT_NE(root, nullptr);

    auto* arr = dynamic_cast<json_data::JsonArray*>(root.get());
    ASSERT_NE(arr, nullptr);

    EXPECT_EQ(arr->size(), 9);

    const auto* v0 = arr->at(0);
    ASSERT_NE(v0, nullptr);
    EXPECT_EQ(v0->type(), json_data::JsonValue::Type::Bool);
    EXPECT_TRUE(v0->asBool());

    const auto* v1 = arr->at(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(v1->type(), json_data::JsonValue::Type::Bool);
    EXPECT_FALSE(v1->asBool());

    const auto* v2 = arr->at(2);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(v2->type(), json_data::JsonValue::Type::Null);

    const auto* v3 = arr->at(3);
    ASSERT_NE(v3, nullptr);
    EXPECT_EQ(v3->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v3->asNumber(), 0.0, 1e-9);

    const auto* v4 = arr->at(4);
    ASSERT_NE(v4, nullptr);
    EXPECT_EQ(v4->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v4->asNumber(), -1.0, 1e-9);

    const auto* v5 = arr->at(5);
    ASSERT_NE(v5, nullptr);
    EXPECT_EQ(v5->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v5->asNumber(), 3.14, 1e-9);

    const auto* v6 = arr->at(6);
    ASSERT_NE(v6, nullptr);
    EXPECT_EQ(v6->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v6->asNumber(), -0.5, 1e-9);

    const auto* v7 = arr->at(7);
    ASSERT_NE(v7, nullptr);
    EXPECT_EQ(v7->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v7->asNumber(), 1e10, 1e5);

    const auto* v8 = arr->at(8);
    ASSERT_NE(v8, nullptr);
    EXPECT_EQ(v8->type(), json_data::JsonValue::Type::Number);
    EXPECT_NEAR(v8->asNumber(), -2.5e-3, 1e-9);
}
