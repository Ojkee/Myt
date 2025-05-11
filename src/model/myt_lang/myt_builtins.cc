#include "../../../include/model/myt_lang/myt_builtins.hpp"

#include "model/myt_lang/myt_object.hpp"

auto MytBuiltins::exec(const std::string& fn_name,
                       const MytObjectArgs& args) noexcept -> MytObjectPtr {
  if (!MytBuiltins::is_in_builtins(fn_name)) {
    return NO_BUILTIN_FN_ERR(fn_name);
  }
  return MytBuiltins::BuiltinFns.at(fn_name)(args);
  return NOT_IMPL_BUILTIN_FN_ERR("exec");
}

auto MytBuiltins::add(const MytObjectArgs& args) noexcept -> MytObjectPtr {
  return NOT_IMPL_BUILTIN_FN_ERR("Add");
}

auto MytBuiltins::pi([[maybe_unused]] const MytObjectArgs& args) noexcept
    -> MytObjectPtr {
  return std::make_shared<ValueObject<FloatType>>(3.1415);
}
