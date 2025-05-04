#ifndef AST_HPP
#define AST_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <typeinfo>

#include "token.hpp"

enum class Precendence : uint8_t {
  Lowest,
  Equals,
  LessGreater,
  Sum,
  Product,
  Prefix,
  Call,
};

namespace AstUtils {
[[nodiscard]] Precendence token_to_precendece(const TokenType& type) noexcept;
}  // namespace AstUtils

struct ParsingError {
  std::string content{};
};

class Expression {
 public:
  virtual ~Expression() = default;
  virtual std::string to_string() const = 0;
  virtual bool equals(const Expression& other) const = 0;

  bool operator==(const Expression& other) const { return this->equals(other); }
};

template <class ExprType>
class ExpressionLiteral : public Expression {
 public:
  ExpressionLiteral() = delete;
  ExpressionLiteral(const ExprType& value) : Expression(), m_value(value) {};

  std::string to_string() const override {
    if constexpr (std::is_same_v<ExprType, bool>) {
      return "bool(" + std::string(m_value ? "true" : "false") + ")";
    } else if constexpr (std::is_same_v<ExprType, int>) {
      return "int(" + std::to_string(m_value) + ")";
    } else if constexpr (std::is_same_v<ExprType, std::string>) {
      return "string(" + m_value + ")";
    }
    return "Unimplemented to_string() of this type";
  }

  bool equals(const Expression& other) const override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    auto otherLiteral = static_cast<const ExpressionLiteral<ExprType>*>(&other);
    return m_value == otherLiteral->get_value();
  }
  ExprType get_value() const noexcept { return m_value; };

 private:
  ExprType m_value;
};

class ExpressionIdentifier : public Expression {
 public:
  ExpressionIdentifier() = delete;
  ExpressionIdentifier(const std::string& value)
      : Expression(), m_name(value) {};

  std::string to_string() const override { return "ident(" + m_name + ")"; }

  bool equals(const Expression& other) const override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    auto otherIdent = static_cast<const ExpressionIdentifier*>(&other);
    return m_name == otherIdent->get_name();
  }

  std::string get_name() const noexcept { return m_name; };

 private:
  std::string m_name{};
};

class ExpressionPrefix : public Expression {
 public:
  ExpressionPrefix() = delete;
  ExpressionPrefix(const Token& prefix_token,
                   std::unique_ptr<Expression>&& expression)
      : m_prefix_token(std::move(prefix_token)),
        m_expression(std::move(expression)) {};

  std::string to_string() const override {
    return "prefix(" + m_prefix_token.literal + m_expression->to_string() + ")";
  };
  bool equals(const Expression& other) const override {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
      return false;
    }
    auto otherPrefix = static_cast<const ExpressionPrefix*>(&other);
    return m_prefix_token == otherPrefix->get_prefix_token() &&
           *m_expression == otherPrefix->get_expression();
  }

  Token get_prefix_token() const noexcept { return m_prefix_token; }
  Expression& get_expression() const noexcept { return *m_expression; }

 private:
  Token m_prefix_token;
  std::unique_ptr<Expression> m_expression;
};

class ExpressionInfix : public Expression {
 public:
  ExpressionInfix() = delete;
  ExpressionInfix(std::unique_ptr<Expression>&& lhs,
                  const Token& operator_token,
                  std::unique_ptr<Expression>&& rhs)
      : m_lhs(std::move(lhs)),
        m_operator_token(operator_token),
        m_rhs(std::move(rhs)) {};

 private:
  std::unique_ptr<Expression> m_lhs;
  Token m_operator_token;
  std::unique_ptr<Expression> m_rhs;
};

#endif  // !AST_HPP
