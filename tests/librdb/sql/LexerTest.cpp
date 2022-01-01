#include <gtest/gtest.h>
#include <librdb/sql/Lexer.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace {

std::string get_tokens(const std::string_view input) {
  rdb::sql::Lexer lexer(input);
  std::stringstream out;
  rdb::sql::Token token = lexer.peek();
  do {
    token = lexer.get();
    out << token << '\n';
  } while (token.kind() != rdb::sql::Token::Kind::Eof);
  return out.str();
}

}  // namespace

TEST(LexerSuite, PeekGetTest) {
  rdb::sql::Lexer lexer("My Str\ni123");
  std::stringstream ss;
  ss << lexer.peek() << ' ';
  ss << lexer.peek() << ' ';
  ss << lexer.peek();
  EXPECT_EQ(ss.str(), "Id 'My' Loc=0:0 Id 'My' Loc=0:0 Id 'My' Loc=0:0");
  ss.str("");
  ss << lexer.get() << ' ';
  ss << lexer.peek();
  EXPECT_EQ(ss.str(), "Id 'My' Loc=0:0 Id 'Str' Loc=3:0");
  ss.str("");
  ss << lexer.get() << ' ';
  ss << lexer.get();
  EXPECT_EQ(ss.str(), "Id 'Str' Loc=3:0 Id 'i123' Loc=0:1");
}

TEST(LexerSuite, IdTest) {
  auto tokens = get_tokens("a b2 aselect $");
  const std::string expected_tokens =
      "Id 'a' Loc=0:0\n"
      "Id 'b2' Loc=2:0\n"
      "Id 'aselect' Loc=5:0\n"
      "Unknown '$' Loc=13:0\n"
      "Eof '<EOF>' Loc=14:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, KeywordsTest) {
  auto tokens = get_tokens("SELECT value FROM table");
  const std::string expected_tokens =
      "KwSelect 'SELECT' Loc=0:0\n"
      "Id 'value' Loc=7:0\n"
      "KwFrom 'FROM' Loc=13:0\n"
      "Id 'table' Loc=18:0\n"
      "Eof '<EOF>' Loc=23:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, KeywordsTest2) {
  auto tokens = get_tokens("CREATE TABLE (WHERE, INSERT");
  const std::string expected_tokens =
      "KwCreate 'CREATE' Loc=0:0\n"
      "KwTable 'TABLE' Loc=7:0\n"
      "LBracket '(' Loc=13:0\n"
      "KwWhere 'WHERE' Loc=14:0\n"
      "Comma ',' Loc=19:0\n"
      "KwInsert 'INSERT' Loc=21:0\n"
      "Eof '<EOF>' Loc=27:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, KeywordsTest3) {
  auto tokens = get_tokens("INTO) VALUES DROP DELETE INT REAL TEXT;");
  const std::string expected_tokens =
      "KwInto 'INTO' Loc=0:0\n"
      "RBracket ')' Loc=4:0\n"
      "KwValues 'VALUES' Loc=6:0\n"
      "KwDrop 'DROP' Loc=13:0\n"
      "KwDelete 'DELETE' Loc=18:0\n"
      "KwInt 'INT' Loc=25:0\n"
      "KwReal 'REAL' Loc=29:0\n"
      "KwText 'TEXT' Loc=34:0\n"
      "Semicolon ';' Loc=38:0\n"
      "Eof '<EOF>' Loc=39:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, IntTest) {
  auto tokens = get_tokens("123 -456 -0 +01 01 -abc");
  const std::string expected_tokens =
      "Int '123' Loc=0:0\n"
      "Int '-456' Loc=4:0\n"
      "Int '-0' Loc=9:0\n"
      "Int '+0' Loc=12:0\n"
      "Int '1' Loc=14:0\n"
      "Int '0' Loc=16:0\n"
      "Int '1' Loc=17:0\n"
      "Unknown '-' Loc=19:0\n"
      "Id 'abc' Loc=20:0\n"
      "Eof '<EOF>' Loc=23:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, RealTest) {
  auto tokens = get_tokens("1.5 -5.65 -0.6 +0. 1.23.45");
  const std::string expected_tokens =
      "Real '1.5' Loc=0:0\n"
      "Real '-5.65' Loc=4:0\n"
      "Real '-0.6' Loc=10:0\n"
      "Unknown '+0.' Loc=15:0\n"
      "Real '1.23' Loc=19:0\n"
      "Unknown '.' Loc=23:0\n"
      "Int '45' Loc=24:0\n"
      "Eof '<EOF>' Loc=26:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, StringTest) {
  auto tokens = get_tokens("\"Abc\" \"\" \"Def\n \"Ghi");
  const std::string expected_tokens =
      "String '\"Abc\"' Loc=0:0\n"
      "String '\"\"' Loc=6:0\n"
      "Unknown '\"Def' Loc=9:0\n"
      "Unknown '\"Ghi' Loc=1:1\n"
      "Eof '<EOF>' Loc=5:1\n";
  EXPECT_EQ(expected_tokens, tokens);
}

TEST(LexerSuite, OperationTest) {
  auto tokens = get_tokens("< > = <= >= != !");
  const std::string expected_tokens =
      "Less '<' Loc=0:0\n"
      "Greater '>' Loc=2:0\n"
      "Equal '=' Loc=4:0\n"
      "Less or equal '<=' Loc=6:0\n"
      "Greater or equal '>=' Loc=9:0\n"      
      "Not equal '!=' Loc=12:0\n"
      "Unknown '!' Loc=15:0\n"
      "Eof '<EOF>' Loc=16:0\n";
  EXPECT_EQ(expected_tokens, tokens);
}
