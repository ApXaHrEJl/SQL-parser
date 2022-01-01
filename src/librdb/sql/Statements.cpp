#include <librdb/sql/Statements.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace rdb::sql {

std::string var_to_str(const Value& value) {
  if (const int* i = std::get_if<int>(&value)) {
    return std::to_string(*i);
  }
  if (const float* f = std::get_if<float>(&value)) {
    return std::to_string(*f);
  }
  if (const std::string_view* s = std::get_if<std::string_view>(&value)) {
    return {s->data(), s->size()};
  }
  return "";
}

std::string operation_to_str(Expression::Operation operation) {
  switch (operation) {
    case Expression::Operation::Less:
      return "<";
    case Expression::Operation::Greater:
      return ">";
    case Expression::Operation::LessEq:
      return "<=";
    case Expression::Operation::GreaterEq:
      return ">=";
    case Expression::Operation::Equal:
      return "=";
    case Expression::Operation::NotEqual:
      return "!=";
  }
  return "Unexpected";
}

std::string column_kind_to_str(ColumnDef::Kind kind) {
  switch (kind) {
    case ColumnDef::Kind::Int:
      return "INT";
    case ColumnDef::Kind::Real:
      return "REAL";
    case ColumnDef::Kind::Text:
      return "TEXT";
  }
  return "Unexpected";
}

Statement::~Statement() = default;

std::string DropTableStatement::to_str() const {
  std::stringstream out;
  out << "DROP TABLE " << table_name() << ";";
  return out.str();
}

std::string InsertStatement::to_str() const {
  std::stringstream out;
  out << "INSERT INTO " << table_name() << " ( ";
  for (const auto& column_name : column_names()) {
    out << column_name << " ";
  }
  out << ") VALUES ( ";
  for (const auto& value : values()) {
    out << var_to_str(value) << " ";
  }
  out << ");";
  return out.str();
}

std::string SelectStatement::to_str() const {
  std::stringstream out;
  out << "SELECT ";
  for (const auto& column : column_list()) {
    out << column << " ";
  }
  out << "FROM " << table_name();
  if (expression() != std::nullopt) {
    out << " WHERE " << var_to_str(expression()->first_operand_.value_) << " "
        << operation_to_str(expression()->operation_) << " "
        << var_to_str(expression()->second_operand_.value_);
  }
  out << ";";
  return out.str();
}

std::string DeleteStatement::to_str() const {
  std::stringstream out;
  out << "DELETE FROM " << table_name();
  if (expression() != std::nullopt) {
    out << " WHERE " << var_to_str(expression()->first_operand_.value_) << " "
        << operation_to_str(expression()->operation_) << " "
        << var_to_str(expression()->second_operand_.value_);
  }
  out << ";";
  return out.str();
}

std::string CreateTableStatement::to_str() const {
  std::stringstream out;
  out << "CREATE TABLE " << table_name() << " ( ";
  for (const auto& column_def : column_defs()) {
    out << column_def.column_name_ << " "
        << column_kind_to_str(column_def.kind_) << " ";
  }
  out << ");";
  return out.str();
}

std::ostream& operator<<(std::ostream& os, const Statement& statement) {
  os << statement.to_str();
  return os;
}

}  // namespace rdb::sql

