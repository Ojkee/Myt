#ifndef CELL_HPP
#define CELL_HPP

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "myt_lang/myt_object.hpp"

using MytObjectPtr = std::shared_ptr<MytObject>;

using CellLimitType = uint16_t;

class InvalidCellString : public std::exception {
 public:
  InvalidCellString(const char* msg) : message(msg) {};
  const char* what() const throw() { return message.c_str(); }

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

  bool operator==(const CellPos& other) const {
    return row == other.row && col == other.col;
  }

 private:
  static const CellLimitType max_limit =
      std::numeric_limits<CellLimitType>::max();
  template <typename UnsignedType, typename = typename std::enable_if<
                                       std::is_unsigned_v<UnsignedType>>::type>
  [[nodiscard]] constexpr static bool in_limit_range(
      const UnsignedType& num) noexcept {
    return num <= max_limit;
  }
  [[nodiscard]] CellLimitType convert_letters(
      const std::string_view& valid_format_str,
      const std::string_view& letters) const;
  [[nodiscard]] CellLimitType convert_number(
      const std::string_view& valid_format_str,
      const std::string_view& number) const;
};

template <>
struct std::hash<CellPos> {
  std::size_t operator()(const CellPos& pos) const {
    return std::hash<CellLimitType>()(pos.row) ^
           (std::hash<CellLimitType>()(pos.col) << 1);
  }
};

class DataCell {
 public:
  DataCell() = delete;
  DataCell(const std::string& raw_content, MytObjectPtr&& value)
      : m_raw_content(raw_content), m_evaluated_content(std::move(value)) {};

  DataCell(DataCell&& other) noexcept = default;
  DataCell(const DataCell&) = default;

  [[nodiscard]] std::string get_raw_content() const noexcept;
  void set_raw_content(const std::string& value) noexcept;

  [[nodiscard]] const std::shared_ptr<MytObject> get_evaluated_content()
      const noexcept;
  void set_eval_content(MytObjectPtr&& value) noexcept;

  [[nodiscard]] const std::string to_string() const noexcept {
    return (m_evaluated_content != nullptr) ? m_evaluated_content->to_string()
                                            : "";
  };

  void reset() noexcept {
    m_raw_content.clear();
    m_evaluated_content = std::make_unique<NilObject>();
  }

 private:
  std::string m_raw_content{};
  MytObjectPtr m_evaluated_content{std::make_unique<NilObject>()};
};

#endif  // !CELL_HPP
