#include "frontend/window_utils.hpp"

#include <qobject.h>

#include "backend/myt_lang/cell_pos.hpp"
#include "global_utils/global_utils.hpp"

WindowUtils::WindowUtils(QObject* parent) : QObject(parent) {}

auto WindowUtils::col_idx_to_letter(const CellLimitType& n) const noexcept
    -> QString {
  const auto res_str = GlobalUtils::col_idx_to_letter_str(n);
  return QString::fromStdString(res_str);
}
