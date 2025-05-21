#ifndef WINDOW_CFG_HPP
#define WINDOW_CFG_HPP

#include <QGuiApplication>

class GridConfig : public QObject {
  Q_OBJECT
  Q_PROPERTY(int rowCount READ rowCount CONSTANT)
  Q_PROPERTY(int columnCount READ columnCount CONSTANT)

 public:
  GridConfig(QObject* parent = nullptr) : QObject(parent) {}

  int rowCount() const { return 1000; }
  int columnCount() const { return 1000; }
};

#endif  // !WINDOW_CFG_HPP
