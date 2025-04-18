#include "../../include/model/data_cell.hpp"

DataCell::DataCell() : m_raw_content(""), m_evaluated_content(std::nullopt) {}

DataCell::DataCell(const std::string& raw_content)
    : m_raw_content(raw_content), m_evaluated_content(std::nullopt) {}

std::string DataCell::get_raw_content() const noexcept { return m_raw_content; }

void DataCell::set_raw_content(const std::string& value) noexcept {
  m_raw_content = value;
}

void DataCell::set_eval_content(const MytObject& value) noexcept {
  m_evaluated_content = value;
}

bool DataCell::is_evaluated() const noexcept {
  return m_evaluated_content != std::nullopt;
}
