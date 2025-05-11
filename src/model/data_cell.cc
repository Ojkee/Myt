#include "../../include/model/data_cell.hpp"

#include <cmath>
#include <cstdint>
#include <string_view>

#include "model/myt_lang/myt_object.hpp"

auto CellPos::convert_number(const std::string_view& valid_format_str,
                             const std::string_view& number) const
    -> CellLimitType {
  const auto numbers_ull = std::stoull(number.data());
  if (!CellPos::in_limit_range(numbers_ull)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(max_limit) + "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(numbers_ull);
}

auto CellPos::convert_letters(const std::string_view& valid_format_str,
                              const std::string_view& letters) const
    -> CellLimitType {
  const auto acc = [](const auto& s, const auto& e, uint32_t init,
                      const auto& op) {
    for (auto [it, idx] = std::tuple{s, uint32_t{}}; it != e; it++, idx++) {
      init += op(*it, idx);
    }
    return init;
  };
  const auto index_op = [](const char& c, const auto& idx) {
    return static_cast<uint32_t>(std::pow(26, idx) * (c - 'A' + 1));
  };
  const auto converted = acc(letters.rbegin(), letters.rend(), 0, index_op);
  if (!CellPos::in_limit_range(converted)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(max_limit) + "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(converted);
}

auto DataCell::get_raw_content() const noexcept -> std::string {
  return m_raw_content;
}

auto DataCell::set_raw_content(const std::string& value) noexcept -> void {
  m_raw_content = value;
}

auto DataCell::get_evaluated_content() const noexcept -> const MytObjectPtr {
  return m_evaluated_content;
}

auto DataCell::set_eval_content(MytObjectPtr&& value) noexcept -> void {
  m_evaluated_content = std::move(value);
}
