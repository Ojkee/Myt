#ifndef GLOBAL_UTILS_HPP
#define GLOBAL_UTILS_HPP

#include <string>
#include <type_traits>
#include "backend/myt_lang/cell_pos.hpp"

class GlobalUtils {
 public:
  template <typename T,
            typename = typename std::enable_if_t<std::is_unsigned_v<T>>>
  [[nodiscard]] static auto col_idx_to_letter_str(const T& n) noexcept
      -> std::string;

  [[nodiscard]] static auto pos_to_str(const CellPos& pos) noexcept
      -> const std::string;
};

#endif  // !GLOBAL_UTILS_HPP
