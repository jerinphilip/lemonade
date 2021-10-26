#pragma once

#include "engine_compat.h"
#include "lemonade/lib/logging.h"
#include "lemonade/lib/translator.h"
#include <string>
#include <vector>

namespace lemonade::ibus {

/// Idea here is to maintain an active buffer string.
//
// 1. The first suggestion is the translated text.
// 2. The second suggestion is the raw text the user entered.
class LemonadeEngine : public Engine {
public:
  LemonadeEngine(IBusEngine *engine);
  ~LemonadeEngine(void);

  // virtual functions
  gboolean processKeyEvent(guint keyval, guint keycode, guint modifiers);
  void focusIn(void);
  void focusOut(void);
  void reset(void);
  void enable(void);
  void disable(void);
  void pageUp(void);
  void pageDown(void);
  void cursorUp(void);
  void cursorDown(void);
  gboolean propertyActivate(const gchar *prop_name, guint prop_state);
  void candidateClicked(guint index, guint button, guint state);

private:
  void showSetupDialog(void);

  g::LookupTable generateLookupTable(const std::vector<std::string> &entries);

  void updateBuffer(const std::string &append);
  void refreshTranslation();
  void commit();

  std::string buffer_ = "";
  std::string translationBuffer_ = "";

  Logger logger_;
  Translator translator_;
};

} // namespace lemonade::ibus
