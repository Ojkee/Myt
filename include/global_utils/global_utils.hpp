#ifndef GLOBAL_UTILS_HPP
#define GLOBAL_UTILS_HPP

#include <string>
#include <type_traits>
#include "backend/myt_lang/cell_pos.hpp"

class GlobalUtils {
 public:
  // METHODS
  template <typename T,
            typename = typename std::enable_if_t<std::is_unsigned_v<T>>>
  [[nodiscard]] static auto col_idx_to_letter_str(const T& n) noexcept
      -> const std::string {
    std::string acc = "";
    for (T i = n; i > 0; i /= 26) {
      i -= 1;
      acc = (char)((i % 26) + 'A') + acc;
    }
    return acc;
  }

  template <typename T,
            typename = typename std::enable_if_t<std::is_integral_v<T>>>
  [[nodiscard]] static auto is_in_cell_range(const T& idx) noexcept -> bool {
    return idx > 0 && static_cast<CellLimitType>(idx) < CELL_MAX_LIMIT;
  }

  // CONSTS
  static const CellLimitType CELL_MAX_LIMIT =
      std::numeric_limits<CellLimitType>::max();
};

#endif  // !GLOBAL_UTILS_HPP
