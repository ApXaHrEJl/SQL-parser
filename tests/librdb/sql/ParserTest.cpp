#include <gtest/gtest.h>
#include <librdb/sql/Parser.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace {

std::string dump_statements(rdb::sql::Parser& parser) {
  std::stringstream out;
  const rdb::sql::Parser::Result result = parser.parse_sql_script();
  for (const auto& i : result.script_.statements_) {
    out << *i << "\n";
  }
  for (const auto& i : result.errors_) {
    out << i << "\n";
  }
  return out.str();
}

}  // namespace

TEST(ParserSuite, DropTableTest) {
  rdb::sql::Lexer lexer(
      "DROP TABLE Table;"
      "DROP Table; DROP;"
      "TABLE;");
  rdb::sql::Parser parser(lexer);
  const std::string statements = dump_statements(parser);
  const std::string expected_statements =
      "DROP TABLE Table;\nExpected KwTable, got Id\n"
      "Expected KwTable, got Semicolon\n"
      "Expected statement type\n";
  EXPECT_EQ(expected_statements, statements);
}

TEST(ParserSuite, InsertTest) {
  rdb::sql::Lexer lexer(
      "INSERT INTO Table (Col1, Col2, Col3) VALUES (123, 4.56, \"7B9\");"
      "INSERT INTO TABLE (Column1) VALUES(A);"
      "INSERT INTO Table Column1,Column2 VALUES (123,456);"
      "INSERT INTO Table (C1, C2, C3) VALUES (123 456);");
  rdb::sql::Parser parser(lexer);
  const std::string statements = dump_statements(parser);
  const std::string expected_statements =
      "INSERT INTO Table ( Col1 Col2 Col3 ) VALUES ( 123 4.560000 \"7B9\" );\n"
      "Expected Id, got KwTable\n"
      "Expected LBracket, got Id\n"
      "Expected RBracket, got Int\n";
  EXPECT_EQ(expected_statements, statements);
}

TEST(ParserSuite, SelectTest) {
  rdb::sql::Lexer lexer(
      "SELECT Col1 Col2 Col3 FROM Table WHERE Val <= 5;"
      "SELECT C1 FROM Table;"
      "SELECT C1 FROM Table WHERE;");
  rdb::sql::Parser parser(lexer);
  const std::string statements = dump_statements(parser);
  const std::string expected_statements =
      "SELECT Col1 Col2 Col3 FROM Table WHERE Val <= 5;\n"
      "SELECT C1 FROM Table;\n"
      "Expected Int, Real, String or Id\n";
  EXPECT_EQ(expected_statements, statements);
}

TEST(ParserSuite, DeleteTest) {
  rdb::sql::Lexer lexer(
      "DELETE FROM Table WHERE Val != null;"
      "DELETE FROM Table;"
      "DELETE FROM Table WHERE A;"
      "DELETE FROM A");
  rdb::sql::Parser parser(lexer);
  const std::string statements = dump_statements(parser);
  const std::string expected_statements =
      "DELETE FROM Table WHERE Val != null;\n"
      "DELETE FROM Table;\n"
      "Expected OperationType, got Semicolon\n"
      "Expected Semicolon, got Eof\n";
  EXPECT_EQ(expected_statements, statements);
}

TEST(ParserSuite, CreateTableTest) {
  rdb::sql::Lexer lexer(
      "CREATE TABLE Table (Name1 INT, Name2 REAL, Name3 TEXT);"
      "CREATE TABLE T (Name1 INT);"
      "CREATE TABLE T (INT Name1);");
  rdb::sql::Parser parser(lexer);
  const std::string statements = dump_statements(parser);
  const std::string expected_statements =
      "CREATE TABLE Table ( Name1 INT Name2 REAL Name3 TEXT );\n"
      "CREATE TABLE T ( Name1 INT );\n"
      "Expected Id, got KwInt\n";
  EXPECT_EQ(expected_statements, statements);
}
