#pragma once

#include <librdb/sql/Lexer.hpp>
#include <librdb/sql/Script.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace rdb::sql {

class Parser {
 public:
  struct Result {
    Script script_;
    std::vector<std::string> errors_;
  };

  explicit Parser(Lexer& lexer) : lexer_(lexer) {}

  Result parse_sql_script();
 private:
  StatementPtr parse_sql_statement();
  DropTableStatementPtr parse_drop_table_statement();
  InsertStatementPtr parse_insert_statement();
  SelectStatementPtr parse_select_statement();
  DeleteStatementPtr parse_delete_statement();
  CreateTableStatementPtr parse_create_table_statement();
  
  Value parse_value();
  Operand parse_operand();
  Expression parse_expression();
  ColumnDef parse_column_def();
  
  void panic();
  Token fetch_token(Token::Kind expected_kind);

  Lexer& lexer_;
};

}  // namespace rdb::sql

