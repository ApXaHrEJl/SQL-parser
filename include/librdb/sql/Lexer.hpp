#pragma once

#include <librdb/sql/Location.hpp>
#include <librdb/sql/Token.hpp>
#include <optional>
#include <string_view>

namespace rdb::sql {

class Lexer {
   public:
    explicit Lexer(std::string_view input)
        : input_(input), location_(0, 0, 0) {}

    Token get();
    Token peek();

   private:
    bool eof() const;
    char peek_char() const;
    char get_char();
    void skip_spaces();
    Token get_id_or_kw();
    Token get_punct();
    Token get_number();
    Token get_string();
    Token get_operation();
    Token make_token(Token::Kind kind, const Location& begin) const;
    std::string_view input_;
    Location location_;
    std::optional<Token> next_token_;
};

} // namespace rdb::sql
