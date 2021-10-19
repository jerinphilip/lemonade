#pragma once
#include <ibus.h>

#include "gtypes.h"

namespace lemonade::ibus {

#define IBUS_TYPE_LEMONADE_ENGINE (ibus_lemonade_engine_get_type())

GType ibus_lemonade_engine_get_type(void);

class Engine {
public:
  Engine(IBusEngine *engine);
  virtual ~Engine(void);

  gboolean contentIsPassword();

  // virtual functions
  virtual gboolean processKeyEvent(guint keyval, guint keycode,
                                   guint modifiers) = 0;
  virtual void focusIn(void) = 0;
  virtual void focusOut(void);
#if IBUS_CHECK_VERSION(1, 5, 4)
  virtual void setContentType(guint purpose, guint hints);
#endif
  virtual void reset(void) = 0;
  virtual void enable(void) = 0;
  virtual void disable(void) = 0;
  virtual void pageUp(void) = 0;
  virtual void pageDown(void) = 0;
  virtual void cursorUp(void) = 0;
  virtual void cursorDown(void) = 0;
  virtual gboolean propertyActivate(const gchar *prop_name,
                                    guint prop_state) = 0;
  virtual void candidateClicked(guint index, guint button, guint state) = 0;

protected:
  void commitText(g::Text &text) const {
    ibus_engine_commit_text(m_engine, text);
  }

  void updatePreeditText(g::Text &text, guint cursor, gboolean visible) const {
    ibus_engine_update_preedit_text(m_engine, text, cursor, visible);
  }

  void showPreeditText(void) const { ibus_engine_show_preedit_text(m_engine); }

  void hidePreeditText(void) const { ibus_engine_hide_preedit_text(m_engine); }

  void updateAuxiliaryText(g::Text &text, gboolean visible) const {
    ibus_engine_update_auxiliary_text(m_engine, text, visible);
  }

  void showAuxiliaryText(void) const {
    ibus_engine_show_auxiliary_text(m_engine);
  }

  void hideAuxiliaryText(void) const {
    ibus_engine_hide_auxiliary_text(m_engine);
  }

  void updateLookupTable(g::LookupTable &table, gboolean visible) const {
    ibus_engine_update_lookup_table(m_engine, table, visible);
  }

  void updateLookupTableFast(g::LookupTable &table, gboolean visible) const {
    ibus_engine_update_lookup_table_fast(m_engine, table, visible);
  }

  void showLookupTable(void) const { ibus_engine_show_lookup_table(m_engine); }

  void hideLookupTable(void) const { ibus_engine_hide_lookup_table(m_engine); }

  void clearLookupTable(g::LookupTable &table) const {
    ibus_lookup_table_clear(table);
  }

  void registerProperties(g::PropList &props) const {
    ibus_engine_register_properties(m_engine, props);
  }

  void updateProperty(g::Property &prop) const {
    ibus_engine_update_property(m_engine, prop);
  }

protected:
  g::Pointer<IBusEngine> m_engine; // engine pointer

#if IBUS_CHECK_VERSION(1, 5, 4)
  IBusInputPurpose m_input_purpose;
#endif
};

} // namespace lemonade::ibus
