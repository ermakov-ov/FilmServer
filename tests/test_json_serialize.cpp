
#include <gtest/gtest.h>
#include "../src/json/json_data.h"

TEST(JsonStringToString, SimpleString)
{
    json_data::JsonString s("Inception");
    EXPECT_EQ(s.toString(), "\"Inception\"");
}

TEST(JsonStringToString, WithQuotesInside)
{
    json_data::JsonString s("The Lord of the \"Rings\"");
    EXPECT_EQ(s.toString(), "\"The Lord of the \\\"Rings\\\"\"");
}

TEST(JsonStringToString, WithSlashAndNewline)
{
    json_data::JsonString s("C:\\Films\\Action\nNew");
    EXPECT_EQ(s.toString(), "\"C:\\\\Films\\\\Action\\nNew\"");
}

TEST(JsonNumberToString, IntegerAndDouble)
{
    {
        json_data::JsonNumber n(42);
        EXPECT_EQ(n.toString(), "42");
    }
    {
        json_data::JsonNumber n(3.14);
        EXPECT_EQ(n.toString(), "3.14");
    }
}

TEST(JsonBoolToString, TrueAndFalse)
{
    {
        json_data::JsonBool b(true);
        EXPECT_EQ(b.toString(), "true");
    }
    {
        json_data::JsonBool b(false);
        EXPECT_EQ(b.toString(), "false");
    }
}

TEST(JsonNullToString, NullValue)
{
    json_data::JsonNull n;
    EXPECT_EQ(n.toString(), "null");
}

TEST(JsonObjectToString, EmptyObject)
{
    auto objPtr = json_data::JsonFactory::makeObject();
    EXPECT_EQ(objPtr->toString(), "{}");
}

TEST(JsonObjectToString, WithMembers)
{
    auto objPtr = json_data::JsonFactory::makeObject();
    auto* obj = dynamic_cast<json_data::JsonObject*>(objPtr.get());
    ASSERT_NE(obj, nullptr);

    obj->setMember("title", json_data::JsonFactory::makeString("Inception"));
    obj->setMember("year", json_data::JsonFactory::makeNumber(2010));
    obj->setMember("is_good", json_data::JsonFactory::makeBool(true));

    std::string result = objPtr->toString();

    EXPECT_NE(result.find("\"title\":\"Inception\""), std::string::npos);
    EXPECT_NE(result.find("\"year\":2010"), std::string::npos);
    EXPECT_NE(result.find("\"is_good\":true"), std::string::npos);
    EXPECT_EQ(result.front(), '{');
    EXPECT_EQ(result.back(), '}');
}

TEST(JsonArrayToString, EmptyArray)
{
    auto arrPtr = json_data::JsonFactory::makeArray();
    EXPECT_EQ(arrPtr->toString(), "[]");
}

TEST(JsonArrayToString, ArrayOfObjects)
{
    auto arrPtr = json_data::JsonFactory::makeArray();
    auto* arr = dynamic_cast<json_data::JsonArray*>(arrPtr.get());
    ASSERT_NE(arr, nullptr);

    {
        auto obj1Ptr = json_data::JsonFactory::makeObject();
        auto* obj1 = dynamic_cast<json_data::JsonObject*>(obj1Ptr.get());
        ASSERT_NE(obj1, nullptr);
        obj1->setMember("id", json_data::JsonFactory::makeNumber(1));
        obj1->setMember("name", json_data::JsonFactory::makeString("Alice"));
        arr->pushBack(std::move(obj1Ptr));
    }

    {
        auto obj2Ptr = json_data::JsonFactory::makeObject();
        auto* obj2 = dynamic_cast<json_data::JsonObject*>(obj2Ptr.get());
        ASSERT_NE(obj2, nullptr);
        obj2->setMember("id", json_data::JsonFactory::makeNumber(2));
        obj2->setMember("name", json_data::JsonFactory::makeString("Bob"));
        arr->pushBack(std::move(obj2Ptr));
    }

    std::string result = arrPtr->toString();

    EXPECT_EQ(result.front(), '[');
    EXPECT_EQ(result.back(), ']');
    EXPECT_NE(result.find("\"id\":1"), std::string::npos);
    EXPECT_NE(result.find("\"name\":\"Alice\""), std::string::npos);
}

TEST(JsonValueToString, NestedStructure)
{
    // Актеры
    auto actorsPtr = json_data::JsonFactory::makeArray();
    auto* actors = dynamic_cast<json_data::JsonArray*>(actorsPtr.get());
    ASSERT_NE(actors, nullptr);
    actors->pushBack(json_data::JsonFactory::makeString("Leo"));
    actors->pushBack(json_data::JsonFactory::makeString("Elliot"));

    // Фильм
    auto moviePtr = json_data::JsonFactory::makeObject();
    auto* movie = dynamic_cast<json_data::JsonObject*>(moviePtr.get());
    ASSERT_NE(movie, nullptr);
    movie->setMember("title", json_data::JsonFactory::makeString("Inception"));
    movie->setMember("actors", std::move(actorsPtr));

    // Массив фильмов
    auto moviesPtr = json_data::JsonFactory::makeArray();
    auto* movies = dynamic_cast<json_data::JsonArray*>(moviesPtr.get());
    ASSERT_NE(movies, nullptr);
    movies->pushBack(std::move(moviePtr));

    // Корневой объект
    auto rootPtr = json_data::JsonFactory::makeObject();
    auto* root = dynamic_cast<json_data::JsonObject*>(rootPtr.get());
    ASSERT_NE(root, nullptr);
    root->setMember("movies", std::move(moviesPtr));

    std::string result = rootPtr->toString();

    EXPECT_EQ(result.front(), '{');
    EXPECT_EQ(result.back(), '}');
    EXPECT_NE(result.find("\"movies\":["), std::string::npos);
    EXPECT_NE(result.find("\"title\":\"Inception\""), std::string::npos);
    EXPECT_NE(result.find("\"actors\":[\"Leo\",\"Elliot\"]"), std::string::npos);

    int balance = 0;
    for (char c : result) {
        if (c == '{') ++balance;
        else if (c == '}') --balance;
        if (balance < 0) break;
    }
    EXPECT_EQ(balance, 0);
}
