#ifndef STATE_HPP
#define STATE_HPP

#include <qobject.h>
#include <qtmetamacros.h>
#include <qurl.h>

#include <cstddef>
#include <vector>

#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/page.hpp"

class State : public QObject {
  Q_OBJECT

 public:
  explicit State(QObject* parent = nullptr);

  [[nodiscard]] Q_INVOKABLE QString get_content_by_pos(
      const CellLimitType& col, const CellLimitType& row) noexcept;
  [[nodiscard]] Q_INVOKABLE QString get_raw_content_by_pos(
      const CellLimitType& col, const CellLimitType& row) noexcept;
  Q_INVOKABLE void eval_save(const QString& raw_content,
                             const CellLimitType& col,
                             const CellLimitType& row) noexcept;
  Q_INVOKABLE void log_cells() const noexcept;

 private:
  [[nodiscard]] auto eval_content(const std::string& raw_content) const noexcept
      -> MytObjectPtr;

  std::size_t m_current_page_idx{};
  std::vector<Page> m_pages;
};

#endif  // !STATE_HPP
