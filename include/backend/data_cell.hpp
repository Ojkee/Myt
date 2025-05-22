#ifndef CELL_HPP
#define CELL_HPP

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <string>

#include "myt_lang/myt_object.hpp"

class DataCell {
 public:
  DataCell() = delete;
  DataCell(const std::string& raw_content, MytObjectPtr value)
      : m_raw_content(raw_content), m_evaluated_content(value) {};

  [[nodiscard]] auto get_raw_content() const noexcept -> std::string;
  auto set_raw_content(const std::string& value) noexcept -> void;

  [[nodiscard]] auto get_evaluated_content() const noexcept
      -> const MytObjectPtr;
  auto set_eval_content(MytObjectPtr&& value) noexcept -> void;

  [[nodiscard]] auto to_string() const noexcept -> const std::string {
    return (m_evaluated_content != nullptr) ? m_evaluated_content->to_string()
                                            : "";
  };

  void reset() noexcept {
    m_raw_content.clear();
    m_evaluated_content = std::make_shared<NilObject>();
  }

 private:
  std::string m_raw_content{};
  MytObjectPtr m_evaluated_content{std::make_shared<NilObject>()};
};

#endif  // !CELL_HPP
