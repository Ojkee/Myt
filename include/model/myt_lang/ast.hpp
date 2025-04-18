#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <type_traits>

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
    if (auto otherLiteral =
            dynamic_cast<const ExpressionLiteral<ExprType>*>(&other)) {
      return m_value == otherLiteral->m_value;
    }
    return false;
  };

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
    if (auto otherIdent = dynamic_cast<const ExpressionIdentifier*>(&other)) {
      return m_name == otherIdent->m_name;
    }
    return false;
  }
  std::string get_name() const noexcept { return m_name; };

 private:
  std::string m_name{};
};

#endif  // !AST_HPP
