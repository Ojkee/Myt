#ifndef STATE_HPP
#define STATE_HPP

#include <qobject.h>
#include <qtmetamacros.h>
#include <qurl.h>

#include <cstddef>
#include <vector>

#include "backend/cell_dependencies_handler.hpp"
#include "backend/myt_lang/cell_pos.hpp"
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

  auto flush_dependencies() noexcept -> void {
    m_dependencies_handler.flush_dependencies();
  }
  [[nodiscard]] auto get_dependencies() const noexcept -> const Dependencies {
    return m_dependencies_handler.get_dependencies();
  }
  [[nodiscard]] auto get_dependencies_uses() const noexcept
      -> const Dependencies {
    return m_dependencies_handler.get_dependencies_uses();
  }

 private:
  auto save_data_cell(const CellPos& pos, const DataCell& data_cell) noexcept
      -> void;

  std::size_t m_current_page_idx{};
  std::vector<Page> m_pages;
  DependenciesHandler m_dependencies_handler;
};

#endif  // !STATE_HPP
