#ifndef MYT_OBJECT_HPP
#define MYT_OBJECT_HPP

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "backend/myt_lang/ast.hpp"

#define MS_VO_T(T, value) std::make_shared<ValueObject<T>>(value)
#define MS_T(T, value) std::make_shared<T>(value)
#define D_CAST(T, expr) dynamic_cast<const T*>(expr)

class MytObject;

using MytObjectPtr = std::shared_ptr<MytObject>;

class MytObject {
 public:
  virtual ~MytObject() = default;
  virtual auto to_string() const noexcept -> const std::string = 0;

  auto operator==(const MytObject& other) const noexcept -> bool {
    return this->to_string() == other.to_string();
  }

  virtual auto add(MytObjectPtr other) const noexcept -> MytObjectPtr = 0;
  virtual auto sub(MytObjectPtr other) const noexcept -> MytObjectPtr = 0;
  virtual auto mul(MytObjectPtr other) const noexcept -> MytObjectPtr = 0;
  virtual auto div(MytObjectPtr other) const noexcept -> MytObjectPtr = 0;
};

class ErrorObject : public MytObject {
 public:
  ErrorObject() = delete;
  explicit ErrorObject(const std::string& value) : m_value(value) {};

  [[nodiscard]] auto to_string() const noexcept -> const std::string override {
    return "Error: `" + m_value + "`";
  };

  auto add([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't add Error");
  }
  auto sub([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't sub Error");
  }
  auto mul([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't mul Error");
  }
  auto div([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't div Error");
  }

 private:
  std::string m_value{};
};

class NilObject : public MytObject {
 public:
  explicit NilObject() = default;
  [[nodiscard]] auto to_string() const noexcept -> const std::string override {
    return "Nil";
  };

  auto add([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't add Nil");
  }

  auto sub([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't sub Nil");
  }

  auto mul([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return MS_T(ErrorObject, "Can't mul Nil");
  }

  auto div([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
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

  [[nodiscard]] auto to_string() const noexcept -> const std::string override {
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

#define LAMBDA_OP(op) [](const auto& a, const auto& b) { return a op b; }, "op"

  auto add(MytObjectPtr other) const noexcept -> MytObjectPtr override {
    if constexpr (std::is_same_v<ObjectType, std::string>) {
      if (auto str_obj = D_CAST(ValueObject<std::string>, other.get())) {
        return MS_VO_T(std::string, m_value + str_obj->get_value());
      }
    }
    return numeric_operation(other, LAMBDA_OP(+));
  }

  auto sub(MytObjectPtr other) const noexcept -> MytObjectPtr override {
    return numeric_operation(other, LAMBDA_OP(-));
  }

  auto mul(MytObjectPtr other) const noexcept -> MytObjectPtr override {
    return numeric_operation(other, LAMBDA_OP(*));
  }

  auto div(MytObjectPtr other) const noexcept -> MytObjectPtr override {
    return numeric_operation(other, LAMBDA_OP(/));
  }

  [[nodiscard]] const ObjectType get_value() const noexcept { return m_value; };

 protected:
  ObjectType m_value{};

 private:
  template <typename Fn, typename = std::enable_if<std::is_invocable_v<Fn>>>
  auto numeric_operation(MytObjectPtr other, Fn&& op,
                         const std::string& op_literal) const noexcept
      -> MytObjectPtr {
    if (auto numeric_sub = apply_operation_on_numeric(other, op)) {
      return *numeric_sub;
    }
    return MS_T(ErrorObject, "Invalid types for `" + op_literal + "`");
  }

  template <typename Fn, typename = std::enable_if<std::is_invocable_v<Fn>>>
  auto apply_operation_on_numeric(MytObjectPtr other, Fn&& op) const noexcept
      -> std::optional<MytObjectPtr> {
    if constexpr (std::is_same_v<ObjectType, FloatType>) {
      if (auto float_obj = D_CAST(ValueObject<FloatType>, other.get())) {
        const auto a = static_cast<FloatType>(m_value);
        const auto b = float_obj->get_value();
        return MS_VO_T(FloatType, op(a, b));
      } else if (auto int_obj = D_CAST(ValueObject<int>, other.get())) {
        const auto a = m_value;
        const auto b = static_cast<FloatType>(int_obj->get_value());
        return MS_VO_T(FloatType, op(a, b));
      }
    } else if constexpr (std::is_same_v<ObjectType, int>) {
      if (auto float_obj = D_CAST(ValueObject<FloatType>, other.get())) {
        const auto a = static_cast<FloatType>(m_value);
        const auto b = float_obj->get_value();
        return MS_VO_T(FloatType, op(a, b));
      } else if (auto int_obj = D_CAST(ValueObject<int>, other.get())) {
        const auto a = m_value;
        const auto b = int_obj->get_value();
        return MS_VO_T(int, op(a, b));
      }
    }
    return std::nullopt;
  }
};

class IdentObject : public ValueObject<std::string> {
 public:
  IdentObject() = delete;
  IdentObject(const std::string& value) : ValueObject<std::string>(value) {};

  [[nodiscard]] auto to_string() const noexcept -> const std::string override {
    return m_value;
  };
};

class CellRangeObject : public MytObject {
 public:
  CellRangeObject() = delete;
  explicit CellRangeObject(const std::string& range_str,
                           const std::vector<MytObjectPtr>& cells_range)
      : m_range_str(range_str), m_cells_range(cells_range) {};

  auto to_string() const noexcept -> const std::string override {
    auto new_line_fold = [](std::string lhs, const MytObjectPtr& obj) {
      return std::move(lhs) + "; " + obj->to_string();
    };
    const auto args =
        std::accumulate(std::next(m_cells_range.begin()), m_cells_range.end(),
                        m_cells_range[0]->to_string(), new_line_fold);
    return m_range_str + " ( " + args + " )";
  }

  auto add([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return std::make_shared<ErrorObject>("Cell range add not supported yet");
  }

  auto sub([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return std::make_shared<ErrorObject>("Cell range sub not supported yet");
  }

  auto mul([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return std::make_shared<ErrorObject>("Can't mul cells range");
  }

  auto div([[maybe_unused]] MytObjectPtr other) const noexcept
      -> MytObjectPtr override {
    return std::make_shared<ErrorObject>("Can't div cells range");
  }

  [[nodiscard]] auto get_cells_range() const noexcept
      -> std::vector<MytObjectPtr> {
    return m_cells_range;
  }

 private:
  std::string m_range_str{};
  std::vector<MytObjectPtr> m_cells_range{};
};

#endif  // MYT_OBJECT_HPP{
