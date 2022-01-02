#include "lemonade_engine.h"
#include "engine_compat.h"
#include "lemonade/lib/logging.h"
#include "lemonade/lib/utils.h"
#include <cctype>
#include <string>
#include <vector>

namespace lemonade::ibus {

LemonadeEngine::PropertyRegistry LemonadeEngine::makeProperties() {
  LemonadeEngine::PropertyRegistry registry;

  auto props = [](std::string side, std::string defaultLang) {
    bool first = false;
    std::vector<std::string> LANGS = {"English",  "German",  "Czech",
                                      "Estonian", "Italian", "Spanish"};
    g::PropList langs;
    for (auto &lang : LANGS) {
      std::string key = side + "_" + lang;
      g::Property langProperty =
          g::Property(/*key=*/key.c_str(),
                      /*type=*/PROP_TYPE_RADIO,
                      /*label=*/g::Text(lang),
                      /*icon=*/nullptr,
                      /*tooltip=*/g::Text(lang),
                      /*sensitive=*/TRUE,
                      /*visible=*/TRUE,
                      /*state=*/lang == defaultLang ? PROP_STATE_CHECKED
                                                    : PROP_STATE_UNCHECKED,
                      /*props=*/nullptr);
      langs.append(langProperty);
    }
    return langs;
  };

  registry.emplace_back(/*key=*/"source",
                        /*type=*/PROP_TYPE_MENU,
                        /*label=*/g::Text("source"),
                        /*icon=*/nullptr,
                        /*tooltip=*/g::Text("Source language"),
                        /*sensitive=*/TRUE,
                        /*visible=*/TRUE,
                        /*state=*/PROP_STATE_CHECKED,
                        /*props=*/props("source", "English"));
  registry.emplace_back(/*key=*/"target",
                        /*type=*/PROP_TYPE_MENU,
                        /*label=*/g::Text("target"),
                        /*icon=*/nullptr,
                        /*tooltip=*/g::Text("Target language"),
                        /*sensitive=*/TRUE,
                        /*visible=*/TRUE,
                        /*state=*/PROP_STATE_CHECKED,
                        /*props=*/props("target", "German"));
  return registry;
}

namespace {
std::string getIBUSLoggingDirectory() {
  // FIXME, hardcode for now. IBUS will be fixed separately.
  return fmt::format("{}/.lemonade/ibus.log", std::getenv("HOME"));
}
} // namespace

/* constructor */
LemonadeEngine::LemonadeEngine(IBusEngine *engine)
    : Engine(engine), logger_("ibus-engine", {getIBUSLoggingDirectory()}),
      translator_(/*maxModels=*/4, /*numWorkers=*/1),
      propertyRegistry_(makeProperties()) {
  logger_.log("Lemonade engine started");
  gint i;
  for (auto &property : propertyRegistry_) {
    propList_.append(property);
    logger_.log("Adding property");
  }

  // Hardcode the following for now.
  sourceLang_ = "English";
  targetLang_ = "German";
}

/* destructor */
LemonadeEngine::~LemonadeEngine(void) { hideLookupTable(); }

gboolean LemonadeEngine::processKeyEvent(guint keyval, guint keycode,
                                         guint modifiers) {
  if (contentIsPassword())
    return FALSE;

  if (modifiers & IBUS_RELEASE_MASK) {
    return FALSE;
  }

  // We are skipping any modifiers. Our workflow is simple. Ctrl-Enter key is
  // send.
  if (modifiers & IBUS_CONTROL_MASK && keyval == IBUS_Return) {
    g::Text text(translationBuffer_);
    commitText(text);
    buffer_.clear();
    translationBuffer_.clear();
    hideLookupTable();
    return TRUE;
  }

  // If ctrl modifier or something is else, we let it pass
  if (modifiers & IBUS_CONTROL_MASK) {
    return FALSE;
  }

  gboolean retval = FALSE;
  switch (keyval) {
  case IBUS_space: {
    if (buffer_.empty()) {
      updateBuffer(" ");
      commit();
    } else if (buffer_.back() == ' ') {
      commit();
    } else {
      updateBuffer(" ");
      retval = TRUE;
    }

  } break;
  case IBUS_Return: {
    translationBuffer_ += "\n";
    commit();
    retval = TRUE;
  } break;
  case IBUS_BackSpace: {
    if (!buffer_.empty()) {
      buffer_.pop_back();
      refreshTranslation();
      retval = TRUE;
    }
  } break;
  case IBUS_Left:
  case IBUS_Right:
  case IBUS_Up:
  case IBUS_Down:
    return FALSE;
    break;

  default: {
    if (isprint(static_cast<unsigned char>(keyval))) {
      std::string append;
      append += static_cast<unsigned char>(keyval);
      updateBuffer(append);
      retval = TRUE;
    } else {
      retval = FALSE;
    }
  } break;
  }
  return retval;
}

void LemonadeEngine::updateBuffer(const std::string &append) {
  buffer_ += append;
  refreshTranslation();
}

void LemonadeEngine::refreshTranslation() {
  std::string bufferCopy = buffer_;
  auto translation =
      translator_.btranslate(std::move(bufferCopy), sourceLang_, targetLang_);

  translationBuffer_ = translation.target.text;
  std::vector<std::string> entries = {buffer_};
  g::LookupTable table = generateLookupTable(entries);
  updateLookupTable(table, /*visible=*/!entries.empty());

  if (!buffer_.empty()) {
    cursorPos_ = translationBuffer_.size();
    g::Text preEdit(translationBuffer_);
    updatePreeditText(preEdit, cursorPos_, TRUE);
    showLookupTable();
  }
}

void LemonadeEngine::commit() {
  g::Text text(translationBuffer_);
  commitText(text);
  hideLookupTable();
  buffer_.clear();
  translationBuffer_.clear();

  hideLookupTable();
  cursorPos_ = 0;
  g::Text preEdit("");
  updatePreeditText(preEdit, cursorPos_, TRUE);
}

void LemonadeEngine::focusIn(void) { registerProperties(propList_); }

void LemonadeEngine::focusOut(void) { Engine::focusOut(); }

void LemonadeEngine::reset(void) {}

void LemonadeEngine::enable(void) {}

void LemonadeEngine::disable(void) {}

void LemonadeEngine::pageUp(void) {}

void LemonadeEngine::pageDown(void) {}

void LemonadeEngine::cursorUp(void) {}

void LemonadeEngine::cursorDown(void) {}

inline void LemonadeEngine::showSetupDialog(void) {
  // g_spawn_command_line_async(LIBEXECDIR "/ibus-setup-libzhuyin zhuyin",
  // NULL);
}

gboolean LemonadeEngine::propertyActivate(const char *prop_name,
                                          guint prop_state) {
  if (prop_state == 1) {
    // source_
    // target_
    std::string serialized(prop_name);
    std::string side = serialized.substr(0, 6);
    std::string lang = serialized.substr(7, serialized.size());
    logger_.log(fmt::format("[{}] [{}]", side, lang));
    if (side == "source") {
      sourceLang_ = lang;
    } else {
      targetLang_ = lang;
    }
  }
  return FALSE;
}

void LemonadeEngine::candidateClicked(guint index, guint button, guint state) {}

g::LookupTable
LemonadeEngine::generateLookupTable(const std::vector<std::string> &entries) {
  g::LookupTable lookupTable;
  for (auto &entry : entries) {
    lookupTable.appendCandidate(g::Text(entry));
  }
  return lookupTable;
}

} // namespace lemonade::ibus
