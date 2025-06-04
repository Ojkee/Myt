#include "../../include/global_utils/global_utils.hpp"
#include "backend/myt_lang/cell_pos.hpp"

template <typename T, typename>
auto GlobalUtils::col_idx_to_letter_str(const T& n) noexcept
    -> const std::string {
  std::string acc = "";
  for (T i = n; i > 0; i /= 26) {
    i -= 1;
    acc = (char)((i % 26) + 'A') + acc;
  }
  return acc;
}

auto GlobalUtils::pos_to_str(const CellPos& pos) noexcept -> const std::string {
  return GlobalUtils::col_idx_to_letter_str(pos.col) + std::to_string(pos.row);
};

auto GlobalUtils::is_in_cell_range(const int& idx) noexcept -> bool {
  return idx > 0 && static_cast<CellLimitType>(idx) < max_limit;
}
