#include "../../include/model/data_cell.hpp"

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
