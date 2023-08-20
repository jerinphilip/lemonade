#pragma once
#include "engine_compat.h"
#include "ibus_config.h"
#include <ibus.h>

namespace lemonade {
class Application {
public:
  Application(gboolean ibus);
  void run();

private:
  IBusBus *bus_ = NULL;
  g::SharedPointer<IBusFactory> factory_ = nullptr;
};
} // namespace lemonade
