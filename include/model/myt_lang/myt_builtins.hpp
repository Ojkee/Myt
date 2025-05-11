#ifndef MYT_BUILTIS_HPP
#define MYT_BUILTIS_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "model/myt_lang/myt_object.hpp"

#define NO_BUILTIN_FN_ERR(value)                                              \
  std::make_shared<ErrorObject>("No function named: `" + std::string(value) + \
                                "`")
#define NOT_IMPL_BUILTIN_FN_ERR(value)                              \
  std::make_shared<ErrorObject>("Unimplemented function named: `" + \
                                std::string(value) + "`")

using MytObjectArgs = std::vector<MytObjectPtr>;
using BuiltinFn = std::function<MytObjectPtr(const MytObjectArgs&)>;

class MytBuiltins {
 public:
  MytBuiltins() = delete;

  [[nodiscard]] static auto exec(const std::string& fn_name,
                                 const MytObjectArgs& args) noexcept
      -> MytObjectPtr;

 private:
  [[nodiscard]] static auto is_in_builtins(const std::string& fn_name) noexcept
      -> bool {
    return MytBuiltins::BuiltinFns.find(fn_name) !=
           MytBuiltins::BuiltinFns.end();
  }

  [[nodiscard]] static auto add(const MytObjectArgs& args) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto pi(
      [[maybe_unused]] const MytObjectArgs& args) noexcept -> MytObjectPtr;
  inline static const std::unordered_map<std::string, BuiltinFn> BuiltinFns{
      {"Add", add},
      {"Pi", pi},
  };
};

#endif  // !MYT_BUILTIS_HPP
