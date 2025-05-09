#ifndef MYT_OBJECT_HPP
#define MYT_OBJECT_HPP

#include <cassert>
#include <string>

#include "model/myt_lang/ast.hpp"

class MytObject {
 public:
  virtual ~MytObject() = default;
  virtual const std::string to_string() const noexcept = 0;
  bool operator==(const MytObject& other) const {
    return this->to_string() == other.to_string();
  }
};

template <class ObjectType,
          class = typename std::enable_if<
              std::is_same<ObjectType, bool>::value ||
              std::is_same<ObjectType, int>::value ||
              std::is_same<ObjectType, std::string>::value ||
              std::is_same<ObjectType, FloatType>::value>::type>
class ValueObject : public MytObject {
 public:
  ValueObject() = delete;
  ValueObject(const ObjectType& value) : MytObject(), m_value(value) {};

  [[nodiscard]] const std::string to_string() const noexcept override {
    if constexpr (std::is_same_v<ObjectType, bool>) {
      return m_value ? "true" : "false";
    } else if constexpr (std::is_same_v<ObjectType, int> ||
                         std::is_same_v<ObjectType, FloatType>) {
      return std::to_string(m_value);
    } else if constexpr (std::is_same_v<ObjectType, std::string>) {
      return "\"" + m_value + "\"";
    }
    assert(false && "Unreachable");
  };

  [[nodiscard]] const ObjectType get_value() const noexcept { return m_value; };

 private:
  ObjectType m_value{};
};

class ErrorObject : public MytObject {
 public:
  ErrorObject() = delete;
  ErrorObject(const std::string& value) : m_value(value) {};

  [[nodiscard]] const std::string to_string() const noexcept override {
    return "Error: `" + m_value + "`";
  };

 private:
  std::string m_value{};
};

class NilObject : public MytObject {
 public:
  NilObject() = default;
  [[nodiscard]] const std::string to_string() const noexcept override {
    return "Nil";
  };
};

#endif  // MYT_OBJECT_HPP
