#ifndef MYT_BUILTIS_HPP
#define MYT_BUILTIS_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "model/myt_lang/myt_object.hpp"

#define NOT_IMPL_BUILTIN_FN_ERR(value)                              \
  std::make_shared<ErrorObject>("Unimplemented function named: `" + \
                                std::string(value) + "`")
#define N_ARGS_ERR(value, n_want, n_got)                               \
  std::make_shared<ErrorObject>("Function: `" + std::string(value) +   \
                                "` takes: " + std::to_string(n_want) + \
                                " arguments, got: " + std::to_string(n_got))
#define N_STR_ARGS_ERR(value, n_want, n_got)                         \
  std::make_shared<ErrorObject>("Function: `" + std::string(value) + \
                                "` takes: " + n_want +               \
                                " arguments, got: " + std::to_string(n_got))
#define WRONG_TYPE_ERR(value, want_type, got_type)                      \
  std::make_shared<ErrorObject>(                                        \
      "Wrong type in function: `" + std::string(value) + "` wants: `" + \
      std::string(want_type) + "` got: `" + std::string(got_type) + "`")

#define DP_CAST_VO_T(T, value) std::dynamic_pointer_cast<ValueObject<T>>(value)
#define DP_CAST_T(T, value) std::dynamic_pointer_cast<T>(value)

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

  // NO ARGS
  [[nodiscard]] static auto m_pi(
      [[maybe_unused]] const MytObjectArgs& args) noexcept -> MytObjectPtr;

  // ONE ARG
  [[nodiscard]] static auto m_sqrt(const MytObjectArgs& args) noexcept
      -> MytObjectPtr;

  // MANY ARGS
  [[nodiscard]] static auto m_sum(const MytObjectArgs& args) noexcept
      -> MytObjectPtr;

  inline static const std::unordered_map<std::string, BuiltinFn> BuiltinFns{
      // NO ARGS
      {"Pi", m_pi},

      // ONE ARG
      {"Sqrt", m_sqrt},

      // MANY ARGS
      {"Sum", m_sum},
  };
};

#endif  // !MYT_BUILTIS_HPP
