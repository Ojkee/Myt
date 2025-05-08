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
[[nodiscard]] Precendence token_to_precendece(const TokenType& type) noexcept;
[[nodiscard]] bool same_hash_code(const Expression& lhs,
                                  const Expression& rhs) noexcept;
}  // namespace AstUtils

struct ParsingError {
  std::string content{};
};

class Expression {
 public:
  virtual ~Expression() = default;
  virtual std::string to_string() const = 0;
  virtual bool equals(const Expression& other) const noexcept = 0;

  bool operator==(const Expression& other) const { return this->equals(other); }
};

template <class ExprType, class = typename std::enable_if<
                              std::is_same<ExprType, bool>::value ||
                              std::is_same<ExprType, int>::value ||
                              std::is_same<ExprType, std::string>::value ||
                              std::is_same<ExprType, FloatType>::value>::type>
class ExpressionLiteral : public Expression {
 public:
  ExpressionLiteral() = delete;
  ExpressionLiteral(const ExprType& value) : Expression(), m_value(value) {};

  std::string to_string() const override {
    if constexpr (std::is_same_v<ExprType, bool>) {
      return "bool(" + std::string(m_value ? "true" : "false") + ")";
    } else if constexpr (std::is_same_v<ExprType, int> ||
                         std::is_same_v<ExprType, FloatType>) {
      return "int(" + std::to_string(m_value) + ")";
    } else if constexpr (std::is_same_v<ExprType, std::string>) {
      return "string(" + m_value + ")";
    }
    return "Unimplemented to_string() of this type";
  }
  bool equals(const Expression& other) const noexcept override {
    if (!AstUtils::same_hash_code(*this, other)) return false;
    const auto other_literal =
        static_cast<const ExpressionLiteral<ExprType>*>(&other);
    return m_value == other_literal->get_value();
  }

  const ExprType get_value() const noexcept { return m_value; };

 private:
  ExprType m_value;
};

class ExpressionIdentifier : public Expression {
 public:
  ExpressionIdentifier() = delete;
  ExpressionIdentifier(const Token& name_token) : m_name_token(name_token) {};

  std::string to_string() const override {
    return "ident(" + m_name_token.literal + ")";
  }

  bool equals(const Expression& other) const noexcept override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_ident = static_cast<const ExpressionIdentifier*>(&other);
    return m_name_token == other_ident->get_name_token();
  }

  const Token get_name_token() const noexcept { return m_name_token; };

 private:
  Token m_name_token{};
};

class ExpressionPrefix : public Expression {
 public:
  ExpressionPrefix() = delete;
  ExpressionPrefix(const Token& prefix_token, ExpressionPtr&& expression)
      : m_prefix_token(std::move(prefix_token)),
        m_expression(std::move(expression)) {};

  std::string to_string() const override {
    return "prefix(" + m_prefix_token.literal + m_expression->to_string() + ")";
  };
  bool equals(const Expression& other) const noexcept override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_prefix = static_cast<const ExpressionPrefix*>(&other);
    return m_prefix_token == other_prefix->get_prefix_token() &&
           *m_expression == other_prefix->get_expression();
  }

  const Token get_prefix_token() const noexcept { return m_prefix_token; }
  const Expression& get_expression() const noexcept { return *m_expression; }

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

  std::string to_string() const override {
    return "infix(" + m_lhs->to_string() + m_operator_token.literal +
           m_rhs->to_string() + ")";
  };
  bool equals(const Expression& other) const noexcept override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    const auto other_infix = static_cast<const ExpressionInfix*>(&other);
    return *m_lhs == other_infix->get_lhs_expression() &&
           m_operator_token == other_infix->get_operator_token() &&
           *m_rhs == other_infix->get_rhs_expression();
  }

  const Token get_operator_token() const noexcept { return m_operator_token; }
  const Expression& get_lhs_expression() const noexcept { return *m_lhs; }
  const Expression& get_rhs_expression() const noexcept { return *m_rhs; }

 private:
  ExpressionPtr m_lhs;
  Token m_operator_token;
  ExpressionPtr m_rhs;
};

class ExpressionCell : public Expression {
 public:
  ExpressionCell() = delete;
  ExpressionCell(const Token& cell_token) : m_cell_token(cell_token) {};

  std::string to_string() const override {
    return "cell(" + m_cell_token.literal + ")";
  };
  bool equals(const Expression& other) const noexcept override {
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

  std::string to_string() const override {
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
  bool equals(const Expression& other) const noexcept override {
    if (!AstUtils::same_hash_code(*this, other)) return false;

    const auto other_fn_call = static_cast<const ExpressionFnCall*>(&other);
    return this->to_string() == other_fn_call->to_string();
  }

 private:
  ExpressionPtr m_fn_identifier;
  Arguments m_arguments;
};

#endif  // !AST_HPP
