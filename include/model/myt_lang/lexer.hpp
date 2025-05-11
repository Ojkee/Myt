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
  [[nodiscard]] static auto tokenize(
      const std::string_view& raw_content) noexcept -> std::vector<Token>;
  [[nodiscard]] static auto tokens_to_string(
      const std::vector<Token>& tokens) noexcept -> std::string;

 private:
  [[nodiscard]] constexpr inline static auto is_whitespace(
      const char& c) noexcept -> bool;
  [[nodiscard]] constexpr inline static auto is_identifier_char(
      const char& c) noexcept -> bool;
  [[nodiscard]] constexpr inline static auto is_cell_identifier_char(
      const char& c) noexcept -> bool;
  [[nodiscard]] constexpr inline static auto is_numeric(const char& c) noexcept
      -> bool;
  [[nodiscard]] static auto get_sign_token(
      const std::string_view& content, std::string_view::iterator& cur) noexcept
      -> std::optional<Token>;
  [[nodiscard]] static auto read_forward_if(const std::string_view& content,
                                            std::string_view::iterator& cur_it,
                                            const Condition& cond) noexcept
      -> std::optional<std::string_view>;
  [[nodiscard]] static auto read_cell_indent(
      const std::string_view& content,
      std::string_view::iterator& cur_it) noexcept
      -> std::optional<std::string_view>;
  [[nodiscard]] static auto read_float(
      const std::string_view& content,
      std::string_view::iterator& cur_it) noexcept
      -> std::optional<std::string_view>;
  [[nodiscard]] static auto is_keyword(const std::string& ident) noexcept
      -> bool;
  [[nodiscard]] static auto get_token_type_keyword(
      const std::string& ident) noexcept -> std::optional<TokenType>;
};

#endif  // !LEXER_HPP
