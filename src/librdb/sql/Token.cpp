#include <librdb/sql/Token.hpp>

namespace rdb::sql {

std::string_view kind_to_str(Token::Kind kind) {
  switch (kind) {
    case Token::Kind::Id:
      return "Id";
    case Token::Kind::Int:
      return "Int";
    case Token::Kind::Real:
      return "Real";
    case Token::Kind::String:
      return "String";
    case Token::Kind::Unknown:
      return "Unknown";
    case Token::Kind::Eof:
      return "Eof";
    case Token::Kind::Semicolon:
      return "Semicolon";
    case Token::Kind::Comma:
      return "Comma";
    case Token::Kind::LBracket:
      return "LBracket";
    case Token::Kind::RBracket:
      return "RBracket";
    case Token::Kind::OpLess:
      return "Less";
    case Token::Kind::OpGreater:
      return "Greater";
    case Token::Kind::OpLessEq:
      return "Less or equal";
    case Token::Kind::OpGreaterEq:
      return "Greater or equal";
    case Token::Kind::OpEqual:
      return "Equal";
    case Token::Kind::OpNotEqual:
      return "Not equal";
    case Token::Kind::KwSelect:
      return "KwSelect";
    case Token::Kind::KwFrom:
      return "KwFrom";
    case Token::Kind::KwCreate:
      return "KwCreate";
    case Token::Kind::KwTable:
      return "KwTable";
    case Token::Kind::KwWhere:
      return "KwWhere";
    case Token::Kind::KwInsert:
      return "KwInsert";
    case Token::Kind::KwInto:
      return "KwInto";
    case Token::Kind::KwValues:
      return "KwValues";
    case Token::Kind::KwDelete:
      return "KwDelete";
    case Token::Kind::KwDrop:
      return "KwDrop";
    case Token::Kind::KwInt:
      return "KwInt";
    case Token::Kind::KwReal:
      return "KwReal";
    case Token::Kind::KwText:
      return "KwText";
  }
  return "Unexpected";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
  os << kind_to_str(token.kind()) << " '" << token.text() << "' "
     << "Loc=" << token.location().cols_ << ':' << token.location().rows_;
  return os;
}

}  // namespace rdb::sql
