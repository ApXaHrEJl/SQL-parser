#include <charconv>
#include <librdb/sql/Parser.hpp>
#include <sstream>

namespace rdb::sql {

namespace {

class SyntaxError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

}  // namespace

Parser::Result Parser::parse_sql_script() {
  Parser::Result result;
  while (true) {
    const Token next_token = lexer_.peek();
    if (next_token.kind() == Token::Kind::Eof) {
      break;
    }
    try {
      result.script_.statements_.push_back(parse_sql_statement());
    } catch (const SyntaxError& e) {
      result.errors_.emplace_back(e.what());
      panic();
    }
  }
  return result;
}

StatementPtr Parser::parse_sql_statement() {
  const Token token = lexer_.peek();
  const Token::Kind kind = token.kind();
  if (kind == Token::Kind::KwDrop) {
    return parse_drop_table_statement();
  }
  if (kind == Token::Kind::KwInsert) {
    return parse_insert_statement();
  }
  if (kind == Token::Kind::KwSelect) {
    return parse_select_statement();
  }
  if (kind == Token::Kind::KwDelete) {
    return parse_delete_statement();
  }
  if (kind == Token::Kind::KwCreate) {
    return parse_create_table_statement();
  }
  throw SyntaxError("Expected statement type");
}

DropTableStatementPtr Parser::parse_drop_table_statement() {
  fetch_token(Token::Kind::KwDrop);
  fetch_token(Token::Kind::KwTable);
  const Token table_name = fetch_token(Token::Kind::Id);
  fetch_token(Token::Kind::Semicolon);
  return std::make_unique<const DropTableStatement>(table_name.text());
}

InsertStatementPtr Parser::parse_insert_statement() {
  fetch_token(Token::Kind::KwInsert);
  fetch_token(Token::Kind::KwInto);
  const Token table_name = fetch_token(Token::Kind::Id);

  fetch_token(Token::Kind::LBracket);
  std::vector<std::string_view> column_names;
  column_names.push_back(fetch_token(Token::Kind::Id).text());
  while (lexer_.peek().kind() == Token::Kind::Comma) {
    fetch_token(Token::Kind::Comma);
    column_names.push_back(fetch_token(Token::Kind::Id).text());
  }
  fetch_token(Token::Kind::RBracket);
  fetch_token(Token::Kind::KwValues);
  fetch_token(Token::Kind::LBracket);

  std::vector<Value> values;
  const Value first_value = parse_value();
  values.push_back(first_value);

  while (lexer_.peek().kind() == Token::Kind::Comma) {
    fetch_token(Token::Kind::Comma);
    const Value next_value = parse_value();
    values.push_back(next_value);
  }

  fetch_token(Token::Kind::RBracket);
  fetch_token(Token::Kind::Semicolon);
  return std::make_unique<const InsertStatement>(
      table_name.text(), column_names, values);
}

SelectStatementPtr Parser::parse_select_statement() {
  fetch_token(Token::Kind::KwSelect);

  std::vector<std::string_view> column_list;
  column_list.push_back(fetch_token(Token::Kind::Id).text());
  while (lexer_.peek().kind() == Token::Kind::Id) {
    column_list.push_back(fetch_token(Token::Kind::Id).text());
  }

  fetch_token(Token::Kind::KwFrom);
  const Token table_name = fetch_token(Token::Kind::Id);

  if (lexer_.peek().kind() != Token::Kind::KwWhere) {
    fetch_token(Token::Kind::Semicolon);
    return std::make_unique<const SelectStatement>(
        column_list, table_name.text());
  }

  fetch_token(Token::Kind::KwWhere);
  const Expression expression = parse_expression();
  fetch_token(Token::Kind::Semicolon);

  return std::make_unique<const SelectStatement>(
      column_list, table_name.text(), expression);
}

DeleteStatementPtr Parser::parse_delete_statement() {
  fetch_token(Token::Kind::KwDelete);
  fetch_token(Token::Kind::KwFrom);
  const Token table_name = fetch_token(Token::Kind::Id);

  if (lexer_.peek().kind() != Token::Kind::KwWhere) {
    fetch_token(Token::Kind::Semicolon);
    return std::make_unique<const DeleteStatement>(table_name.text());
  }

  fetch_token(Token::Kind::KwWhere);
  const Expression expression = parse_expression();
  fetch_token(Token::Kind::Semicolon);

  return std::make_unique<const DeleteStatement>(table_name.text(), expression);
}

CreateTableStatementPtr Parser::parse_create_table_statement() {
  fetch_token(Token::Kind::KwCreate);
  fetch_token(Token::Kind::KwTable);
  const Token table_name = fetch_token(Token::Kind::Id);

  fetch_token(Token::Kind::LBracket);
  std::vector<ColumnDef> column_defs;
  const ColumnDef first_column_def = parse_column_def();
  column_defs.push_back(first_column_def);

  while (lexer_.peek().kind() == Token::Kind::Comma) {
    fetch_token(Token::Kind::Comma);
    const ColumnDef next_column_def = parse_column_def();
    column_defs.push_back(next_column_def);
  }

  fetch_token(Token::Kind::RBracket);
  fetch_token(Token::Kind::Semicolon);

  return std::make_unique<const CreateTableStatement>(
      table_name.text(), column_defs);
}

Value Parser::parse_value() {
  const Token token = lexer_.peek();
  if (token.kind() == Token::Kind::Int) {
    fetch_token(Token::Kind::Int);
    constexpr int BITNESS = 10;
    return int(std::strtol(token.text().data(), nullptr, BITNESS));
  }
  if (token.kind() == Token::Kind::Real) {
    fetch_token(Token::Kind::Real);
    return float(std::strtod(token.text().data(), nullptr));
  }
  if (token.kind() == Token::Kind::String) {
    fetch_token(Token::Kind::String);
    return token.text();
  }
  throw SyntaxError("Expected Int, Real or String");
}

Operand Parser::parse_operand() {
  const Token token = lexer_.peek();
  if (token.kind() == Token::Kind::Int) {
    fetch_token(Token::Kind::Int);
    constexpr int BITNESS = 10;
    const auto value = static_cast<int>(std::strtol(token.text().data(), nullptr, BITNESS));
    return Operand(Operand::Kind::Int, value);
  }
  if (token.kind() == Token::Kind::Real) {
    fetch_token(Token::Kind::Real);
    const auto value = static_cast<float>(std::strtod(token.text().data(), nullptr));
    return Operand(Operand::Kind::Real, value);
  }
  if (token.kind() == Token::Kind::String) {
    fetch_token(Token::Kind::String);
    const std::string_view value = token.text();
    return Operand(Operand::Kind::Text, value);
  }

  if (token.kind() == Token::Kind::Id) {
    fetch_token(Token::Kind::Id);
    const std::string_view value = token.text();
    return Operand(Operand::Kind::Id, value);
  }

  throw SyntaxError("Expected Int, Real, String or Id");
}

Expression Parser::parse_expression() {
  const Operand first_operand = parse_operand();

  const Token token = lexer_.peek();
  static const std::unordered_map<Token::Kind, Expression::Operation> operation_to_kind = {
      {Token::Kind::OpLess, Expression::Operation::Less},
      {Token::Kind::OpGreater, Expression::Operation::Greater},
      {Token::Kind::OpLessEq, Expression::Operation::LessEq},
      {Token::Kind::OpGreaterEq, Expression::Operation::GreaterEq},
      {Token::Kind::OpEqual, Expression::Operation::Equal},
      {Token::Kind::OpNotEqual, Expression::Operation::NotEqual}};      
      
  auto it = operation_to_kind.find(token.kind());
  if (it == (operation_to_kind.end())) {
    throw SyntaxError(
        "Expected OperationType, got " +
        std::string(kind_to_str(token.kind())));
  }
  fetch_token(it->first);

  const Operand second_operand = parse_operand();
  return Expression(first_operand, it->second, second_operand);
}

ColumnDef Parser::parse_column_def() {

  const Token name = fetch_token(Token::Kind::Id);
  const Token token = lexer_.peek();
  static const std::unordered_map<Token::Kind, ColumnDef::Kind> token_to_kind = {
      {Token::Kind::KwInt, ColumnDef::Kind::Int},
      {Token::Kind::KwReal, ColumnDef::Kind::Real},
      {Token::Kind::KwText, ColumnDef::Kind::Text}};      

  auto it = token_to_kind.find(token.kind());
  if (it == (token_to_kind.end())) {
    throw SyntaxError(
        "Expected INT, REAL or TEXT, got " +
        std::string(kind_to_str(token.kind())));
  }

  fetch_token(it->first);
  return ColumnDef(name.text(), it->second);
}

void Parser::panic() {
  while (true) {
    const Token preview = lexer_.get();
    if ((preview.kind() == Token::Kind::Eof) ||
        (preview.kind() == Token::Kind::Semicolon)) {
      break;
    }
  }
}

Token Parser::fetch_token(Token::Kind expected_kind) {
  const Token token = lexer_.peek();
  if (token.kind() != expected_kind) {
    throw SyntaxError(
        "Expected " + std::string(kind_to_str(expected_kind)) + ", got " +
        std::string(kind_to_str(token.kind())));
  }
  return lexer_.get();
}

}  // namespace rdb::sql

