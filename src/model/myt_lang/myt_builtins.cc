#include "../../../include/model/myt_lang/myt_builtins.hpp"

#include <cmath>
#include <memory>

#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/myt_object.hpp"

#define MYT_PI 3.141592653589793238462643383279502884197

auto MytBuiltins::exec(const std::string& fn_name,
                       const MytObjectArgs& args) noexcept -> MytObjectPtr {
  if (!MytBuiltins::is_in_builtins(fn_name)) {
    return std::make_shared<ErrorObject>("No function named: `" + fn_name +
                                         "`");
  }
  return MytBuiltins::BuiltinFns.at(fn_name)(args);
}

// NO ARGS

auto MytBuiltins::m_pi([[maybe_unused]] const MytObjectArgs& args) noexcept
    -> MytObjectPtr {
  if (!args.empty()) {
    return N_ARGS_ERR("Pi", 0, args.size());
  }
  constexpr auto ret_pi = static_cast<FloatType>(MYT_PI);
  return std::make_shared<ValueObject<FloatType>>(ret_pi);
}

// ONE ARG

auto MytBuiltins::m_sqrt(const MytObjectArgs& args) noexcept -> MytObjectPtr {
  if (args.size() != 1) {
    return N_ARGS_ERR("Pi", 1, args.size());
  }
  constexpr auto exec_sqrt = [](const auto& v) {
    const auto value_double = static_cast<double>(v);
    const auto sqrt_double = std::sqrt(value_double);
    const auto ret_float = static_cast<FloatType>(sqrt_double);
    return MS_VO_T(FloatType, ret_float);
  };

  if (auto int_obj = DP_CAST_VO_T(int, args[0])) {
    return exec_sqrt(int_obj->get_value());
  } else if (auto float_obj = DP_CAST_VO_T(FloatType, args[0])) {
    return exec_sqrt(float_obj->get_value());
  }
  return WRONG_TYPE_ERR("sqrt", "int/float", args[0]->to_string());
}

// MANY ARGS

auto MytBuiltins::m_sum(const MytObjectArgs& args) noexcept -> MytObjectPtr {
  return NOT_IMPL_BUILTIN_FN_ERR("Sum");
}
