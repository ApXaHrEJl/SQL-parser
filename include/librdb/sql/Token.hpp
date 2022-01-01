#pragma once

#include <librdb/sql/Location.hpp>
#include <ostream>
#include <string_view>
#include <unordered_map>

namespace rdb::sql {

class Token {
 public:
  enum class Kind {
    Id,
    Int,
    Real,
    String,
    Unknown,
    Eof,
    Semicolon,
    Comma,
    LBracket,
    RBracket,
    OpLess,
    OpGreater,
    OpLessEq,
    OpGreaterEq,
    OpEqual,
    OpNotEqual,
    KwSelect,
    KwFrom,
    KwCreate,
    KwTable,
    KwWhere,
    KwInsert,
    KwInto,
    KwValues,
    KwDelete,
    KwDrop,
    KwInt,
    KwReal,
    KwText
  };
  Token(Kind kind, std::string_view text, const Location location)
      : kind_(kind), text_(text), location_(location) {}

  Kind kind() const { return kind_; }
  
  std::string_view text() const { return text_; }
  
  const Location& location() const { return location_; }

 private:
  Kind kind_;
  std::string_view text_;
  Location location_;
};

std::string_view kind_to_str(Token::Kind kind);

std::ostream& operator<<(std::ostream& os, const Token& token);

}  // namespace rdb::sql
