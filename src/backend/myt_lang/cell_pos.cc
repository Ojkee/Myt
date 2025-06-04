#include "../../../include/backend/myt_lang/cell_pos.hpp"

#include <cmath>
#include "global_utils/global_utils.hpp"

auto CellPos::convert_number(const std::string_view& valid_format_str,
                             const std::string_view& number) const
    -> CellLimitType {
  const auto numbers_ull = std::stoull(number.data());
  if (!GlobalUtils::is_in_cell_range(numbers_ull)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(GlobalUtils::CELL_MAX_LIMIT) +
                     "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(numbers_ull);
}

auto CellPos::convert_letters(const std::string_view& valid_format_str,
                              const std::string_view& letters) const
    -> CellLimitType {
  const auto acc = [](const auto& s, const auto& e, uint32_t init,
                      const auto& op) {
    for (auto [it, idx] = std::tuple{s, uint32_t{}}; it != e; ++it, ++idx) {
      init += op(*it, idx);
    }
    return init;
  };
  const auto index_op = [](const char& c, const auto& idx) {
    return static_cast<uint32_t>(std::pow(26, idx) * (c - 'A' + 1));
  };
  const auto converted = acc(letters.crbegin(), letters.crend(), 0, index_op);
  if (!GlobalUtils::is_in_cell_range(converted)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(GlobalUtils::CELL_MAX_LIMIT) +
                     "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(converted);
}

auto CellPos::to_string() const noexcept -> const std::string {
  return GlobalUtils::col_idx_to_letter_str(col) + std::to_string(row);
}
