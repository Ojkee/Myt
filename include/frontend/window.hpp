#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <qqmlapplicationengine.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <limits>

#include "backend/myt_lang/cell_pos.hpp"
#include "backend/state.hpp"
#include "frontend/window_utils.hpp"

class Window {
 public:
  Window() = delete;
  Window(int argc, char* argv[])
      : m_wu(), m_state(), m_app(argc, argv), m_engine() {
    const QUrl url{QStringLiteral("qrc:/include/frontend/qml/Main.qml")};

    QObject::connect(
        &m_engine, &QQmlApplicationEngine::objectCreated, &m_app,
        [url](QObject* obj, const QUrl& objUrl) {
          if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    m_engine.rootContext()->setContextProperty("windowUtils", &m_wu);
    m_engine.rootContext()->setContextProperty("windowState", &m_state);

    m_engine.rootContext()->setContextProperty("colrowLimit", m_colrowLimit);
    m_engine.load(url);
  }

  auto exec() -> int { return m_app.exec(); }

 private:
  constexpr static CellLimitType m_colrowLimit =
      std::numeric_limits<CellLimitType>::max();

  WindowUtils m_wu;
  State m_state;

  QGuiApplication m_app;
  QQmlApplicationEngine m_engine;
};

#endif  // !WINDOW_HPP
