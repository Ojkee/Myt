#include "frontend/window_utils.hpp"

#include <qobject.h>

#include "backend/myt_lang/cell_pos.hpp"

WindowUtils::WindowUtils(QObject* parent) : QObject(parent) {}

auto WindowUtils::col_idx_to_letter(const CellLimitType& n) const noexcept
    -> QString {
  QString acc = "";
  for (CellLimitType i = n; i > 0; i /= 26) {
    i -= 1;
    acc = static_cast<QChar>((i % 26) + 'A') + acc;
  }
  return acc;
}
