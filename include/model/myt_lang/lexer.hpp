#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

#include "token.hpp"

using Condition = std::function<bool(const char&)>;

class Lexer {
 public:
  Lexer();
  [[nodiscard]] static std::vector<Token> tokenize(
      const std::string_view& raw_content) noexcept;
  [[nodiscard]] static std::string tokens_to_string(
      const std::vector<Token>& tokens) noexcept;

 private:
  [[nodiscard]] constexpr inline static bool is_whitespace(
      const char& c) noexcept;
  [[nodiscard]] constexpr inline static bool is_identifier_char(
      const char& c) noexcept;
  [[nodiscard]] constexpr inline static bool is_cell_identifier_char(
      const char& c) noexcept;
  [[nodiscard]] constexpr inline static bool is_numeric(const char& c) noexcept;
  [[nodiscard]] static std::optional<Token> get_sign_token(
      const std::string_view& content,
      std::string_view::iterator& cur) noexcept;
  [[nodiscard]] static std::optional<std::string_view> read_forward_if(
      const std::string_view& content, std::string_view::iterator& cur_it,
      const Condition& cond) noexcept;
  [[nodiscard]] static std::optional<std::string_view> read_cell_indent(
      const std::string_view& content, std::string_view::iterator& cur_it);
  [[nodiscard]] static bool is_keyword(const std::string& ident) noexcept;
  [[nodiscard]] static std::optional<TokenType> get_token_type_keyword(
      const std::string& ident) noexcept;
};

#endif  // !LEXER_HPP
