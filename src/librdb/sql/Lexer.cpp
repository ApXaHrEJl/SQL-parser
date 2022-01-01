#include <cassert>
#include <cctype>
#include <librdb/sql/Lexer.hpp>
#include <librdb/sql/Token.hpp>
#include <optional>
#include <string_view>

namespace rdb::sql {

Token Lexer::get() {
  if (next_token_) {
    Token token(*next_token_);
    next_token_.reset();
    return token;
  }

  skip_spaces();
  if (eof()) {
    return Token(Token::Kind::Eof, "<EOF>", location_);
  }

  const auto next_char = peek_char();

  if (isalpha(next_char) != 0) {
    return get_id_or_kw();
  }

  switch (next_char) {
    case '+':
    case '-':
      return get_number();
    case '"':
      return get_string();
    case '!':
    case '<':
    case '>':
    case '=':
      return get_operation();
    case ';':
    case ',':
    case '(':
    case ')':
      return get_punct();
    default:
      break;  // do nothing;
  }

  if (isdigit(next_char) != 0) {
    return get_number();
  }

  const auto begin(location_);
  get_char();
  return make_token(Token::Kind::Unknown, begin);
}

Token Lexer::peek() {
  if (next_token_) {
    return *next_token_;
  }
  next_token_ = get();
  return *next_token_;
}

bool Lexer::eof() const {
  return location_.offset_ == input_.size();
}

char Lexer::peek_char() const {
  assert(!eof());
  return input_[location_.offset_];
}

char Lexer::get_char() {
  assert(!eof());
  if (input_[location_.offset_] == '\n') {
    location_.cols_ = 0;
    ++location_.rows_;
  } else {
    ++location_.cols_;
  }
  return input_[location_.offset_++];
}

void Lexer::skip_spaces() {
  while (!eof() && (isspace(peek_char()) != 0)) {
    get_char();
  }
}

Token Lexer::get_id_or_kw() {
  const auto begin(location_);
  while (!eof() && (isalnum(peek_char()) != 0)) {
    get_char();
  }
  const auto end(location_);

  const std::string_view text =
      input_.substr(begin.offset_, end.offset_ - begin.offset_);

  static const std::unordered_map<std::string_view, Token::Kind> text_to_kind =
      {
          {"SELECT", Token::Kind::KwSelect},
          {"FROM", Token::Kind::KwFrom},
          {"CREATE", Token::Kind::KwCreate},
          {"TABLE", Token::Kind::KwTable},
          {"WHERE", Token::Kind::KwWhere},
          {"INSERT", Token::Kind::KwInsert},
          {"INTO", Token::Kind::KwInto},
          {"VALUES", Token::Kind::KwValues},
          {"DELETE", Token::Kind::KwDelete},
          {"DROP", Token::Kind::KwDrop},
          {"INT", Token::Kind::KwInt},
          {"REAL", Token::Kind::KwReal},
          {"TEXT", Token::Kind::KwText},
      };

  auto it = text_to_kind.find(text);

  if (it != (text_to_kind.end())) {
    return Token(it->second, text, begin);
  }
  return Token(Token::Kind::Id, text, begin);
}

Token Lexer::get_punct() {
  const auto begin(location_);
  const auto next_char = get_char();
  static const std::unordered_map<char, Token::Kind> char_to_kind = {
      {';', Token::Kind::Semicolon},
      {',', Token::Kind::Comma},
      {'(', Token::Kind::LBracket},
      {')', Token::Kind::RBracket}};

  auto it = char_to_kind.find(next_char);

  if (it != (char_to_kind.end())) {
    return make_token(it->second, begin);
  }
  return make_token(Token::Kind::Unknown, begin);
}

Token Lexer::get_number() {
  const auto begin(location_);
  if ((peek_char() == '+') || (peek_char() == '-')) {
    get_char();
    if (eof() || (!eof() && (isdigit(peek_char()) == 0))) {
      return make_token(Token::Kind::Unknown, begin);
    }
  }
  if (peek_char() == '0') {
    get_char();
    if (!eof() && (isdigit(peek_char()) != 0)) {
      return make_token(Token::Kind::Int, begin);
    }
  }

  while (!eof() && (isdigit(peek_char()) != 0)) {
    get_char();
  }
  if (!eof() && (peek_char() == '.')) {
    get_char();
    if ((!eof() && (isdigit(peek_char()) == 0)) || eof()) {
      return make_token(Token::Kind::Unknown, begin);
    }
    while (!eof() && (isdigit(peek_char()) != 0)) {
      get_char();
    }
    return make_token(Token::Kind::Real, begin);
  }
  return make_token(Token::Kind::Int, begin);
}

Token Lexer::get_string() {
  const auto begin = location_;

  assert(peek_char() == '"');
  get_char();

  while (!eof() && (peek_char() != '"') && (peek_char() != '\n')) {
    get_char();
  }

  if (eof() || peek_char() != '"') {
    return make_token(Token::Kind::Unknown, begin);
  }

  get_char();
  return make_token(Token::Kind::String, begin);
}

Token Lexer::get_operation() {
  assert(!eof());
  const auto begin(location_);
  const auto first_char = get_char();

  if (first_char == '!') {
    if (eof() || peek_char() != '=') {
      return make_token(Token::Kind::Unknown, begin);
    }
    get_char();
    return make_token(Token::Kind::OpNotEqual, begin);
  }
  if (!eof() && peek_char() == '=') {
    get_char();
    if (first_char == '<') {
      return make_token(Token::Kind::OpLessEq, begin);
    }
    if (first_char == '>') {
      return make_token(Token::Kind::OpGreaterEq, begin);
    }
  }
  static const std::unordered_map<char, Token::Kind> char_to_kind = {
      {'<', Token::Kind::OpLess},
      {'>', Token::Kind::OpGreater},
      {'=', Token::Kind::OpEqual}};

  auto it = char_to_kind.find(first_char);
  return make_token(it->second, begin);
}

Token Lexer::make_token(Token::Kind kind, const Location& begin) const {
  const auto length = location_.offset_ - begin.offset_;
  const auto text = input_.substr(begin.offset_, length);
  return Token(kind, text, begin);
}

}  // namespace rdb::sql
