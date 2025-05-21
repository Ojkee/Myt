#ifndef AST_HPP
#define AST_HPP

#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "token.hpp"

class Expression;

using FloatType = float;

using ExpressionPtr = std::unique_ptr<Expression>;
using Arguments = std::vector<ExpressionPtr>;

enum class Precendence : uint8_t {
  Lowest,
  Equals,
  LessGreater,
  Sum,
  Product,
  CellRange,
  Prefix,
  Call,
};

namespace AstUtils {
[[nodiscard]] auto token_to_precendece(const TokenType& type) noexcept
    -> Precendence;
[[nodiscard]] auto same_hash_code(const Expression& lhs,
                                  const Expression& rhs) noexcept -> bool;
}  // namespace AstUtils

struct ParsingError {
  std::string content{};
};

class Expression {
 public:
  virtual ~Expression() = default;
  virtual auto to_string() const -> std::string = 0;
  virtual auto equals(const Expression& other) const noexcept -> bool = 0;

  auto operator==(const Expression& other) const -> bool {
    return this->equals(other);
  }
};

template <class ExprLiteralT,
          class = typename std::enable_if<
              std::is_same<ExprLiteralT, bool>::value ||
              std::is_same<ExprLiteralT, int>::value ||
              std::is_same<ExprLiteralT, std::string>::value ||
              std::is_same<ExprLiteralT, FloatType>::value>::type>
class ExpressionLiteral : public Expression {
 public:
  ExpressionLiteral() = delete;
  ExpressionLiteral(const ExprLiteralT& value)
      : Expression(), m_value(value) {};

  auto to_string() const -> std::string override {
    if constexpr (std::is_same_v<ExprLiteralT, bool>) {
      return "bool(" + std::string(m_value ? "true" : "false") + ")";
    } else if constexpr (std::is_same_v<ExprLiteralT, int> ||
                         std::is_same_v<ExprLiteralT, FloatType>) {
      return "int(" + std::to_string(m_value) + ")";
    } else if constexpr (std::is_same_v<ExprLiteralT, std::string>) {
      return "string(" + m_value + ")";
    }
    return "Unimplemented to_string() of this type";
  }

  auto equals(const Expression& other) const noexcept -> bool override {
    if (!AstUtils::same_hash_code(*this, other)) return false;
    const auto other_literal =
        static_cast<const ExpressionLiteral<ExprLiteralT>*>(&other);
    return m_value == other_literal->get_value();
  }

  auto get_value() const noexcept -> const ExprLiteralT { return m_value; };

 private:
  ExprLiteralT m_value;
};

class ExpressionIdentifier : public Expression {
 public:
  ExpressionIdentifier() = delete;
  ExpressionIdentifier(const Token& name_token) : m_name_token(name_token) {};

  auto to_string() const -> std::string override {
    return "ident(" + m_name_token.literal + ")";
  }

  auto equals(const Expression& other) const noexcept -> bool override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_ident = static_cast<const ExpressionIdentifier*>(&other);
    return m_name_token == other_ident->get_name_token();
  }

  auto get_name_token() const noexcept -> const Token { return m_name_token; };

 private:
  Token m_name_token{};
};

class ExpressionPrefix : public Expression {
 public:
  ExpressionPrefix() = delete;
  ExpressionPrefix(const Token& prefix_token, ExpressionPtr&& expression)
      : m_prefix_token(std::move(prefix_token)),
        m_expression(std::move(expression)) {};

  auto to_string() const -> std::string override {
    return "prefix(" + m_prefix_token.literal + m_expression->to_string() + ")";
  };

  auto equals(const Expression& other) const noexcept -> bool override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_prefix = static_cast<const ExpressionPrefix*>(&other);
    return m_prefix_token == other_prefix->get_prefix_token() &&
           *m_expression == other_prefix->get_expression();
  }

  auto get_prefix_token() const noexcept -> const Token {
    return m_prefix_token;
  }
  auto get_expression() const noexcept -> const Expression& {
    return *m_expression;
  }

 private:
  Token m_prefix_token;
  ExpressionPtr m_expression;
};

class ExpressionInfix : public Expression {
 public:
  ExpressionInfix() = delete;
  ExpressionInfix(ExpressionPtr&& lhs, const Token& operator_token,
                  ExpressionPtr&& rhs)
      : m_lhs(std::move(lhs)),
        m_operator_token(operator_token),
        m_rhs(std::move(rhs)) {};

  auto to_string() const -> std::string override {
    return "infix(" + m_lhs->to_string() + m_operator_token.literal +
           m_rhs->to_string() + ")";
  };

  auto equals(const Expression& other) const noexcept -> bool override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_infix = static_cast<const ExpressionInfix*>(&other);
    return *m_lhs == other_infix->get_lhs_expression() &&
           m_operator_token == other_infix->get_operator_token() &&
           *m_rhs == other_infix->get_rhs_expression();
  }

  auto get_operator_token() const noexcept -> const Token {
    return m_operator_token;
  }

  auto get_lhs_expression() const noexcept -> const Expression& {
    return *m_lhs;
  }

  auto get_rhs_expression() const noexcept -> const Expression& {
    return *m_rhs;
  }

 private:
  ExpressionPtr m_lhs;
  Token m_operator_token;
  ExpressionPtr m_rhs;
};

class ExpressionCell : public Expression {
 public:
  ExpressionCell() = delete;
  ExpressionCell(const Token& cell_token) : m_cell_token(cell_token) {};

  auto to_string() const -> std::string override {
    return "cell(" + m_cell_token.literal + ")";
  };

  auto equals(const Expression& other) const noexcept -> bool override {
    if (!AstUtils::same_hash_code(*this, other)) return false;

    const auto other_cell = static_cast<const ExpressionCell*>(&other);
    return m_cell_token == other_cell->get_cell_token();
  }

  const Token get_cell_token() const noexcept { return m_cell_token; };

 private:
  Token m_cell_token{};
};

class ExpressionFnCall : public Expression {
 public:
  ExpressionFnCall() = delete;
  ExpressionFnCall(ExpressionPtr&& fn_identifier, Arguments&& arguments)
      : m_fn_identifier(std::move(fn_identifier)),
        m_arguments(std::move(arguments)) {}

  auto to_string() const -> std::string override {
    constexpr auto comma_sep_fold = [](const auto& lhs, const auto& arg_expr) {
      return lhs + ", " + arg_expr->to_string();
    };
    const auto args_to_str = [this, &comma_sep_fold]() {
      if (m_arguments.size() == 0) return std::string{};
      if (m_arguments.size() == 1) return m_arguments[0]->to_string();
      return std::accumulate(std::next(m_arguments.begin()), m_arguments.end(),
                             m_arguments[0]->to_string(), comma_sep_fold);
    };
    return "fn_call(" + m_fn_identifier->to_string() + "(" + args_to_str() +
           "))";
  };

  auto equals(const Expression& other) const noexcept -> bool override {
    if (!AstUtils::same_hash_code(*this, other)) return false;
    return this->to_string() == other.to_string();
  }

  auto get_fn_identifier() const noexcept -> const Expression& {
    return *m_fn_identifier;
  }

  auto get_arguments() const noexcept -> const Arguments& {
    return m_arguments;
  }

 private:
  ExpressionPtr m_fn_identifier;
  Arguments m_arguments;
};

#endif  // !AST_HPP
