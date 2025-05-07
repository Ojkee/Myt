#include "../../../include/model/myt_lang/lexer.hpp"

#include <cctype>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

std::vector<Token> Lexer::tokenize(
    const std::string_view& raw_content) noexcept {
  const auto not_qoute = [](const char& c) { return c != '\"'; };

  std::vector<Token> tokens{};
  auto i = raw_content.begin();
  while (i < raw_content.end()) {
    if (is_whitespace(*i)) {
      i++;
      continue;
    } else if (auto single_char_tok = Lexer::get_sign_token(raw_content, i)) {
      tokens.emplace_back(*single_char_tok);
    } else if (*i == '\"') {
      i++;
      if (auto str = Lexer::read_forward_if(raw_content, i, not_qoute)) {
        tokens.emplace_back(Token{TokenType::String, std::string(*str)});
      }
      i++;
    } else if (auto cell_ident = Lexer::read_cell_indent(raw_content, i)) {
      const auto token_value = std::string(*cell_ident);
      const auto token = Token{TokenType::CellIdentifier, token_value};
      tokens.emplace_back(token);
    } else if (auto ident = Lexer::read_forward_if(raw_content, i,
                                                   Lexer::is_identifier_char)) {
      if (auto keywordType =
              Lexer::get_token_type_keyword(std::string(*ident))) {
        const auto token = Token{*keywordType, std::string(*ident)};
        tokens.emplace_back(token);
      } else {
        const auto token = Token{TokenType::Identifier, std::string(*ident)};
        tokens.emplace_back(token);
      }
    } else if (auto float_num = Lexer::read_float(raw_content, i)) {
      const auto token = Token{TokenType::Float, std::string(*float_num)};
      tokens.emplace_back(token);
    } else if (auto num =
                   Lexer::read_forward_if(raw_content, i, Lexer::is_numeric)) {
      const auto token = Token{TokenType::Int, std::string(*num)};
      tokens.emplace_back(token);
    } else {
      const auto token = Token{TokenType::Illegal, "Illegal"};
      tokens.emplace_back(token);
    }
    i++;
  }

  tokens.emplace_back(Token{TokenType::EndOfCell, "EOC"});
  return tokens;
}

std::string Lexer::tokens_to_string(const std::vector<Token>& tokens) noexcept {
  if (tokens.empty()) return "No tokens";
  if (tokens.size() == 1) return std::string(tokens[0]);
  auto new_line_fold = [](std::string lhs, const Token& tok) {
    return std::move(lhs) + "\n" + std::string(tok);
  };
  return std::accumulate(std::next(tokens.begin()), tokens.end(),
                         std::string(tokens[0]), new_line_fold);
}

constexpr inline bool Lexer::is_whitespace(const char& c) noexcept {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

constexpr inline bool Lexer::is_identifier_char(const char& c) noexcept {
  return std::isalpha(c) || c == '_';
}

constexpr inline bool Lexer::is_cell_identifier_char(const char& c) noexcept {
  return std::isalpha(c) && std::isupper(c);
}

constexpr inline bool Lexer::is_numeric(const char& c) noexcept {
  return std::isdigit(c);
}

std::optional<Token> Lexer::get_sign_token(
    const std::string_view& content, std::string_view::iterator& cur) noexcept {
  switch (*cur) {
    case '=':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Eq, "=="};
      }
      return Token{TokenType::Assign, "="};
    case '!':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::NotEq, "!="};
      }
      return Token{TokenType::Bang, "!"};
    case '+':
      return Token{TokenType::Plus, "+"};
    case '-':
      return Token{TokenType::Minus, "-"};
    case '*':
      return Token{TokenType::Asterisk, "*"};
    case '/':
      return Token{TokenType::Slash, "/"};
    case ',':
      return Token{TokenType::Comma, ","};
    case ':':
      return Token{TokenType::Colon, ":"};
    case '(':
      return Token{TokenType::LParen, "("};
    case ')':
      return Token{TokenType::RParen, ")"};
    case '>':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Ge, ">="};
      }
      return Token{TokenType::Gt, ">"};
    case '<':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Le, "<="};
      }
      return Token{TokenType::Lt, "<"};
  }
  return std::nullopt;
}

std::optional<std::string_view> Lexer::read_forward_if(
    const std::string_view& content, std::string_view::iterator& cur_it,
    const Condition& cond) noexcept {
  auto start = cur_it;
  std::size_t len{0};
  while (cur_it != content.end() && cond(*cur_it)) {
    len++;
    cur_it++;
  }
  if (len > 0) {
    cur_it--;
    return std::string_view{start, len};
  }
  return std::nullopt;
}

std::optional<std::string_view> Lexer::read_cell_indent(
    const std::string_view& content,
    std::string_view::iterator& cur_it) noexcept {
  bool started_letter_idx{false}, started_number_idx{false};

  const auto start = cur_it;
  auto it = cur_it;
  constexpr auto char_or_num = [](const auto& c) {
    return Lexer::is_cell_identifier_char(c) || Lexer::is_numeric(c);
  };

  for (; it != content.end() && char_or_num(*it); ++it) {
    if (Lexer::is_cell_identifier_char(*it)) {
      if (started_number_idx)
        return std::nullopt;
      else if (!started_letter_idx)
        started_letter_idx = true;
    } else {
      if (!started_letter_idx)
        return std::nullopt;
      else if (!started_number_idx) {
        if (*it == '0') return std::nullopt;
        started_number_idx = true;
      }
    }
  }
  if (started_letter_idx && started_number_idx) {
    auto len = static_cast<std::size_t>(it - start);
    cur_it = --it;
    return std::string_view{start, len};
  }
  return std::nullopt;
}

std::optional<std::string_view> Lexer::read_float(
    const std::string_view& content,
    std::string_view::iterator& cur_it) noexcept {
  const auto start = cur_it;
  auto it = cur_it;
  bool seen_dot{false};
  bool seen_num{false};
  const auto num_or_point = [](const auto& c) {
    return Lexer::is_numeric(c) || c == '.';
  };

  for (; it != content.end() && num_or_point(*it); ++it) {
    if (*it == '.') {
      if (seen_dot) return std::nullopt;
      seen_dot = true;
    } else if (is_numeric(*it)) {
      seen_num = true;
    }
  }
  if (!seen_dot || !seen_num) return std::nullopt;

  auto len = static_cast<std::size_t>(it - start);
  cur_it = --it;
  return std::string_view{start, len};
}

bool Lexer::is_keyword(const std::string& ident) noexcept {
  return TokenUtils::keywordsLookup.find(ident) !=
         TokenUtils::keywordsLookup.end();
}

std::optional<TokenType> Lexer::get_token_type_keyword(
    const std::string& ident) noexcept {
  if (!is_keyword(ident)) return std::nullopt;
  return TokenUtils::keywordsLookup.at(ident);
}
