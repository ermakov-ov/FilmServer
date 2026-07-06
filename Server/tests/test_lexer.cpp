
#include <gtest/gtest.h>
#include "../src/json/lexer.h"
#include "../src/json/stream.h"
#include "../src/common/common.h"
#include <memory>
#include <cstring>

using namespace parser;

// Вспомогательная функция для создания потока из строки
StreamBuffer makeStream(const std::string& data)
{
    auto ptr = std::make_unique<char[]>(data.size() + 1);
    std::memcpy(ptr.get(), data.c_str(), data.size());
    ptr[data.size()] = '\0';
    return StreamBuffer(std::move(ptr), data.size());
}

TEST(LexerTest, SkipWhitespaceAndPeek)
{
    StreamBuffer stream = makeStream("   {");
    LexerString lexer(stream);

    // peekToken должен пропустить пробелы и вернуть CurlyOpen
    EXPECT_EQ(lexer.peekToken(), TokenType::TT_CurlyOpen);
    // Позиция должна быть ровно на '{'
    EXPECT_EQ(stream.position(), 3);
}

TEST(LexerTest, NextTokenConsumes)
{
    // Твоя семантика: nextToken берёт токен и двигает позицию
    StreamBuffer stream = makeStream("{   [");
    LexerString lexer(stream);

    EXPECT_EQ(lexer.peekToken(), TokenType::TT_CurlyOpen);
    // После '{' и пропущенных пробелов курсор должен быть на '['
    EXPECT_EQ(stream.position(), 0);

    EXPECT_EQ(lexer.nextToken(), TokenType::TT_BracketOpen);
    EXPECT_EQ(stream.position(), 4);
}

TEST(LexerTest, ReadStringWithEscape)
{
    // Проверяем экранирование и то, что лишние символы не съедены
    StreamBuffer stream = makeStream("\"a\\nb\" extra");
    LexerString lexer(stream);

    EXPECT_EQ(lexer.peekToken(), TokenType::TT_String);

    std::string result = lexer.readString();

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 'a');
    EXPECT_EQ(result[1], '\n');
    EXPECT_EQ(result[2], 'b');

    EXPECT_EQ(stream.position(), 5);
}

TEST(LexerTest, ThrowOnUnclosedString)
{
    StreamBuffer stream = makeStream("\"no_end");
    LexerString lexer(stream);

    EXPECT_THROW({
        lexer.readString();
    }, parser::LexerError);
}

TEST(LexerTest, IsEofAndPeekEdgeCases)
{
    // Проверка граничных условий
    StreamBuffer stream = makeStream("{}");
    LexerString lexer(stream);

    EXPECT_EQ(lexer.peekToken(), TokenType::TT_CurlyOpen);
    EXPECT_EQ(lexer.nextToken(), TokenType::TT_CurlyClose);

    lexer.nextToken();

    // Теперь поток должен быть в конце
    EXPECT_TRUE(stream.isEof());

    EXPECT_EQ(lexer.nextToken(), TokenType::TT_Unknown);
}
