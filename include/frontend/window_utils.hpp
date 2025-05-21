#ifndef WINDOW_UTILS_HPP
#define WINDOW_UTILS_HPP

#include <qqmlintegration.h>

#include <QObject>
#include <QString>

#include "backend/myt_lang/cell_pos.hpp"

class WindowUtils : public QObject {
  Q_OBJECT

 public:
  explicit WindowUtils(QObject* parent = nullptr);

  [[nodiscard]] Q_INVOKABLE QString
  col_idx_to_letter(const CellLimitType& n) const noexcept;

 private:
};

#endif  // !WINDOW_UTILS_HPP
