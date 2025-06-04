#ifndef CELL_POS_HPP
#define CELL_POS_HPP

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <string>
#include <string_view>

using CellLimitType = uint16_t;

class InvalidCellString : public std::exception {
 public:
  InvalidCellString(const char* msg) : message(msg) {};
  auto what() const throw() -> const char* { return message.c_str(); }

 private:
  std::string message{};
};

struct CellPos {
  CellPos() = delete;
  CellPos(const CellLimitType& col, const CellLimitType& row)
      : col(col), row(row) {};
  CellPos(const std::string_view& valid_format_str) {
    auto it = valid_format_str.begin();
    std::size_t len{0};
    while (std::isalpha(*it++)) {
      len++;
    }
    const std::string_view letters =
        std::string_view{valid_format_str.begin(), len};
    col = CellPos::convert_letters(valid_format_str, letters);
    const std::string_view numbers = std::string_view{
        valid_format_str.begin() + len, valid_format_str.size() - len};
    row = CellPos::convert_number(valid_format_str, numbers);
  };

  CellLimitType col{};
  CellLimitType row{};

  auto operator==(const CellPos& other) const -> bool {
    return row == other.row && col == other.col;
  }

  auto operator!=(const CellPos& other) const -> bool {
    return !(*this == other);
  }

  auto operator<(const CellPos& other) const -> bool {
    return (row == other.row) ? col < other.col : row < other.row;
  }

  auto operator<=(const CellPos& other) const -> bool {
    return (*this < other) || (*this == other);
  }

  [[nodiscard]] auto to_string() const noexcept -> const std::string;

 private:
  [[nodiscard]] auto convert_letters(const std::string_view& valid_format_str,
                                     const std::string_view& letters) const
      -> CellLimitType;
  [[nodiscard]] auto convert_number(const std::string_view& valid_format_str,
                                    const std::string_view& number) const
      -> CellLimitType;
};

template <>
struct std::hash<CellPos> {
  auto operator()(const CellPos& pos) const -> std::size_t {
    return std::hash<CellLimitType>()(pos.row) ^
           (std::hash<CellLimitType>()(pos.col) << 1);
  }
};

#endif  // !CELL_POS_HPP
