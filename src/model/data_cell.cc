#include "../../include/model/data_cell.hpp"

#include <cmath>
#include <cstdint>
#include <string_view>

CellLimitType CellPos::convert_number(const std::string_view& valid_format_str,
                                      const std::string_view& number) const {
  const auto numbers_ull = std::stoull(number.data());
  if (!CellPos::in_limit_range(numbers_ull)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(max_limit) + "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(numbers_ull);
}

CellLimitType CellPos::convert_letters(const std::string_view& valid_format_str,
                                       const std::string_view& letters) const {
  const auto acc = [](const auto& s, const auto& e, uint32_t init,
                      const auto& op) {
    for (auto [it, idx] = std::tuple{s, uint32_t{}}; it != e; it++, idx++) {
      init += op(*it, idx);
    }
    return init;
  };
  const auto index_op = [](const char& c, const auto& idx) {
    return (uint32_t)(std::pow(26, idx) * (c - 'A' + 1));
  };
  const auto converted = acc(letters.rbegin(), letters.rend(), 0, index_op);
  if (!CellPos::in_limit_range(converted)) {
    const auto msg = "Invalid range in: `" + std::string(valid_format_str) +
                     "` max: `" + std::to_string(max_limit) + "`";
    throw InvalidCellString(msg.c_str());
  }
  return static_cast<CellLimitType>(converted);
}

DataCell::DataCell() : m_raw_content(""), m_evaluated_content(std::nullopt) {}

DataCell::DataCell(const std::string& raw_content)
    : m_raw_content(raw_content), m_evaluated_content(std::nullopt) {}

std::string DataCell::get_raw_content() const noexcept { return m_raw_content; }

void DataCell::set_raw_content(const std::string& value) noexcept {
  m_raw_content = value;
}

void DataCell::set_eval_content(MytObjectPtr&& value) noexcept {
  m_evaluated_content = std::move(value);
}

bool DataCell::is_evaluated() const noexcept {
  return m_evaluated_content != std::nullopt;
}
