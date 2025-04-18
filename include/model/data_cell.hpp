#ifndef CELL_HPP
#define CELL_HPP

#include <cstdint>
#include <optional>
#include <string>

#include "myt_lang/myt_object.hpp"

struct CellPos {
  uint8_t row;
  uint8_t col;

  bool operator==(const CellPos& other) const {
    return row == other.row && col == other.col;
  }
};

template <>
struct std::hash<CellPos> {
  std::size_t operator()(const CellPos& pos) const {
    return std::hash<uint8_t>()(pos.row) ^ (std::hash<uint8_t>()(pos.col) << 1);
  }
};

class DataCell {
 public:
  DataCell();
  DataCell(const std::string& raw_content);

  [[nodiscard]] std::string get_raw_content() const noexcept;
  void set_raw_content(const std::string& value) noexcept;

  [[nodiscard]] bool is_evaluated() const noexcept;
  void set_eval_content(const MytObject& value) noexcept;

 private:
  std::string m_raw_content{};
  std::optional<MytObject> m_evaluated_content{};
};

#endif  // !CELL_HPP
