#include "app.hpp"

auto main(int argc, char *argv[]) -> int {
  MytApp app{argc, argv};
  return app.exec();
}
