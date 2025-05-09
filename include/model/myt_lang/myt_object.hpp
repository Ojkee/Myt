#ifndef MYT_OBJECT_HPP
#define MYT_OBJECT_HPP

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include "model/myt_lang/ast.hpp"

#define MS_VO_T(T, value) std::make_shared<ValueObject<T>>(value)
#define MS_T(T, value) std::make_shared<T>(value)

class MytObject;

using MytObjectPtr = std::shared_ptr<MytObject>;

class MytObject {
 public:
  virtual ~MytObject() = default;
  virtual const std::string to_string() const noexcept = 0;

  bool operator==(const MytObject& other) const noexcept {
    return this->to_string() == other.to_string();
  }

  virtual MytObjectPtr add(MytObjectPtr other) const noexcept = 0;
  virtual MytObjectPtr sub(MytObjectPtr other) const noexcept = 0;
  virtual MytObjectPtr mul(MytObjectPtr other) const noexcept = 0;
  virtual MytObjectPtr div(MytObjectPtr other) const noexcept = 0;
};

class ErrorObject : public MytObject {
 public:
  ErrorObject() = delete;
  explicit ErrorObject(const std::string& value) : m_value(value) {};

  [[nodiscard]] const std::string to_string() const noexcept override {
    return "Error: `" + m_value + "`";
  };

  MytObjectPtr add(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't add Error");
  }
  MytObjectPtr sub(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't sub Error");
  }
  MytObjectPtr mul(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't mul Error");
  }
  MytObjectPtr div(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't div Error");
  }

 private:
  std::string m_value{};
};

class NilObject : public MytObject {
 public:
  explicit NilObject() = default;
  [[nodiscard]] const std::string to_string() const noexcept override {
    return "Nil";
  };

  MytObjectPtr add(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't add Nil");
  }
  MytObjectPtr sub(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't sub Nil");
  }
  MytObjectPtr mul(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't mul Nil");
  }
  MytObjectPtr div(
      [[maybe_unused]] MytObjectPtr other) const noexcept override {
    return MS_T(ErrorObject, "Can't div Nil");
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
  explicit ValueObject(const ObjectType& value)
      : MytObject(), m_value(value) {};

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

  MytObjectPtr add(MytObjectPtr other) const noexcept override {
    if constexpr (std::is_same_v<ObjectType, std::string>) {
      if (auto str_obj =
              dynamic_cast<const ValueObject<std::string>*>(other.get())) {
        return MS_VO_T(std::string, m_value + str_obj->get_value());
      }
    }
    return numeric_operation(
        other, [](const auto& a, const auto& b) { return a + b; }, "+");
  }

  MytObjectPtr sub(MytObjectPtr other) const noexcept override {
    return numeric_operation(
        other, [](const auto& a, const auto& b) { return a - b; }, "-");
  }

  MytObjectPtr mul(MytObjectPtr other) const noexcept override {
    return numeric_operation(
        other, [](const auto& a, const auto& b) { return a * b; }, "*");
  }

  MytObjectPtr div(MytObjectPtr other) const noexcept override {
    return numeric_operation(
        other, [](const auto& a, const auto& b) { return a / b; }, "/");
  }

  [[nodiscard]] const ObjectType get_value() const noexcept { return m_value; };

 private:
  ObjectType m_value{};

  template <typename Fn, typename = std::enable_if<std::is_invocable_v<Fn>>>
  MytObjectPtr numeric_operation(MytObjectPtr other, Fn&& op,
                                 const std::string op_literal) const noexcept {
    if (auto numeric_sub = apply_operation_on_numeric(other, op)) {
      return *numeric_sub;
    }
    return MS_T(ErrorObject, "Invalid types for `" + op_literal + "`");
  }

  template <typename Fn, typename = std::enable_if<std::is_invocable_v<Fn>>>
  std::optional<MytObjectPtr> apply_operation_on_numeric(
      MytObjectPtr other, Fn&& op) const noexcept {
    if constexpr (std::is_same_v<ObjectType, FloatType>) {
      if (auto float_obj =
              dynamic_cast<const ValueObject<FloatType>*>(other.get())) {
        auto result =
            op(static_cast<FloatType>(m_value), float_obj->get_value());
        return MS_VO_T(FloatType, result);
      } else if (auto int_obj =
                     dynamic_cast<const ValueObject<int>*>(other.get())) {
        auto result = op(m_value, static_cast<FloatType>(int_obj->get_value()));
        return MS_VO_T(FloatType, result);
      }
    } else if constexpr (std::is_same_v<ObjectType, int>) {
      if (auto float_obj =
              dynamic_cast<const ValueObject<FloatType>*>(other.get())) {
        auto result =
            op(static_cast<FloatType>(m_value), float_obj->get_value());
        return MS_VO_T(FloatType, result);
      } else if (auto int_obj =
                     dynamic_cast<const ValueObject<int>*>(other.get())) {
        auto result = op(m_value, int_obj->get_value());
        return MS_VO_T(int, result);
      }
    }
    return std::nullopt;
  }
};

#endif  // MYT_OBJECT_HPP{
