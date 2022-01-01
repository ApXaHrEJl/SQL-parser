#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

namespace rdb::sql {

using Value = std::variant<int, float, std::string_view>;

std::string var_to_str(const Value& value);

typedef struct Operand {
  enum class Kind { Int, Real, Text, Id };

 public:
  Operand(Kind kind, Value value) : kind_(kind), value_(value) {}

  Kind kind_;
  Value value_;
} Operand;

typedef struct Expression {
  enum class Operation { Less, Greater, LessEq, GreaterEq, Equal, NotEqual };
  Expression(Operand first_operand, Operation operation, Operand second_operand)
      : first_operand_(first_operand),
        operation_(operation),
        second_operand_(second_operand) {}

  Operand first_operand_;
  Operation operation_;
  Operand second_operand_;
} Expression;

std::string operation_to_str(Expression::Operation operation);

typedef struct ColumnDef {
  enum class Kind { Int, Real, Text };

 public:
  ColumnDef(std::string_view column_name, Kind kind) : column_name_(column_name), kind_(kind) {}

  std::string_view column_name_;
  Kind kind_;
} ColumnDef;

std::string column_kind_to_str(ColumnDef::Kind kind);

class Statement {
 public:
  virtual ~Statement() = 0;
  virtual std::string to_str() const = 0;
};

using StatementPtr = std::unique_ptr<const Statement>;

class DropTableStatement : public Statement {
 public:
  explicit DropTableStatement(std::string_view table_name)
      : table_name_(table_name) {}

  std::string_view table_name() const { return table_name_; }
  virtual std::string to_str() const;

 private:
  std::string_view table_name_;
};

using DropTableStatementPtr = std::unique_ptr<const DropTableStatement>;

class InsertStatement : public Statement {
 public:
  InsertStatement(
      const std::string_view table_name,
      const std::vector<std::string_view>& column_names,
      const std::vector<Value>& values)
      : table_name_(table_name), column_names_(column_names), values_(values) {}

  const std::string_view table_name() const { return table_name_; }
  const std::vector<std::string_view>& column_names() const {
    return column_names_;
  }
  const std::vector<Value>& values() const { return values_; }
  std::string to_str() const override;

 private:
  std::string_view table_name_;
  std::vector<std::string_view> column_names_;
  std::vector<Value> values_;
};

using InsertStatementPtr = std::unique_ptr<const InsertStatement>;

class SelectStatement : public Statement {
 public:
  SelectStatement(
      const std::vector<std::string_view>& column_list,
      std::string_view table_name,
      std::optional<Expression> expression = std::nullopt)
      : column_list_(column_list),
        table_name_(table_name),
        expression_(expression) {}

  const std::vector<std::string_view>& column_list() const {
    return column_list_;
  }
  const std::string_view table_name() const { return table_name_; }
  const std::optional<Expression> expression() const { return expression_; }
  virtual std::string to_str() const;

 private:
  std::vector<std::string_view> column_list_;
  std::string_view table_name_;
  std::optional<Expression> expression_;
};

using SelectStatementPtr = std::unique_ptr<const SelectStatement>;

class DeleteStatement : public Statement {
 public:
  DeleteStatement(
      std::string_view table_name,
      std::optional<Expression> expression = std::nullopt)
      : table_name_(table_name), expression_(expression) {}

  const std::string_view table_name() const { return table_name_; }
  const std::optional<Expression> expression() const { return expression_; }
  std::string to_str() const override;

 private:
  std::string_view table_name_;
  std::optional<Expression> expression_;
};

using DeleteStatementPtr = std::unique_ptr<const DeleteStatement>;

class CreateTableStatement : public Statement {
 public:
  CreateTableStatement(
      std::string_view table_name,
      const std::vector<ColumnDef>& column_defs)
      : table_name_(table_name), column_defs_(column_defs) {}

  const std::string_view table_name() const { return table_name_; }
  const std::vector<ColumnDef>& column_defs() const { return column_defs_; }
  std::string to_str() const override;

 private:
  std::string_view table_name_;
  std::vector<ColumnDef> column_defs_;
};

using CreateTableStatementPtr = std::unique_ptr<const CreateTableStatement>;

std::ostream& operator<<(std::ostream& os, const Statement& statement);

}  // namespace rdb::sql

