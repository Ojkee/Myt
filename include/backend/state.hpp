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
  Q_PROPERTY(int editingCol READ editingCol WRITE setEditingCol NOTIFY
                 editingColChanged)
  Q_PROPERTY(int editingRow READ editingRow WRITE setEditingRow NOTIFY
                 editingRowChanged)

 public:
  explicit State(QObject* parent = nullptr);

  int editingCol() const { return m_editingCol; }
  int editingRow() const { return m_editingRow; }
  [[nodiscard]] Q_INVOKABLE QString
  get_content_by_pos(const CellLimitType& col,
                     const CellLimitType& row) noexcept;
  [[nodiscard]] Q_INVOKABLE QString
  get_raw_content_by_pos(const CellLimitType& col,
                         const CellLimitType& row) noexcept;
  Q_INVOKABLE void eval_save(const QString& raw_content,
                             const CellLimitType& col,
                             const CellLimitType& row) noexcept;
  Q_INVOKABLE void log_cells() const noexcept;

  auto flush_dependencies() noexcept -> void;
  [[nodiscard]] auto get_dependencies() const noexcept
      -> const DependenciesHandler::Dependencies;
  [[nodiscard]] auto get_dependencies_uses() const noexcept
      -> const DependenciesHandler::Dependencies;

 public slots:
  void setEditingCol(int col);
  void setEditingRow(int row);

 signals:
  void requestCellUpdate(int col, int row);
  void editingColChanged();
  void editingRowChanged();

 private:
  auto evaluate(const std::string& content, const CellPos& pos) noexcept
      -> void;

  auto save_data_cell(const CellPos& pos, const DataCell& data_cell) noexcept
      -> void;
  template <class Container>
  auto set_cyclic_dependencies_errors(const Container& pos) noexcept -> void;

  template <class Container>
  [[nodiscard]] static auto build_cell_pos_str(const Container& c)
      -> std::string;
  auto reeval_affected(const CellPos& pos) noexcept -> void;

  int m_editingCol = -1;
  int m_editingRow = -1;

  std::size_t m_current_page_idx{};
  std::vector<Page> m_pages;
  DependenciesHandler m_dependencies_handler;
};

#endif  // !STATE_HPP
