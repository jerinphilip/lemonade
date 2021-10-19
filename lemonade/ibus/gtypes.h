#pragma once

#include <glib-object.h>
#include <string>

// RAII wrap automating some things for a GLIB pointer object
namespace g {
template <typename T> struct Pointer {
public:
  Pointer(T *p = NULL) : pointer_(NULL) { set(p); }

  ~Pointer(void) { set(NULL); }

  Pointer<T> &operator=(T *p) {
    set(p);
    return *this;
  }

  // Note: Forbid operations on something declared const.
  Pointer<T> &operator=(const Pointer<T> &p) = delete; /*{
    set(p.pointer_);
    return *this;
  }
  */

  Pointer<T>(Pointer<T> &&p) {
    set(p.pointer_);
    p.set(NULL);
  }

  Pointer<T>(const Pointer<T> &p) = delete;

  const T *operator->(void) const { return pointer_; }

  T *operator->(void) { return pointer_; }

  operator T *(void) const { return pointer_; }

  operator gboolean(void) const { return pointer_ != NULL; }

private:
  T *pointer_;

  void set(T *p) {
    if (pointer_) {
      g_object_unref(pointer_);
    }

    pointer_ = p;
    if (p) {
#ifdef DEBUG
      g_debug("%s, floating = %d", G_OBJECT_TYPE_NAME(p),
              g_object_is_floating(p));
#endif
      g_object_ref_sink(p);
    }
  }
};

class Object {
protected:
  template <typename T> Object(T *p) : pointer_((GObject *)p) {
    g_assert(get<GObject *>() != NULL);
  }

  operator GObject *(void) const { return pointer_; }

  template <typename T> T *get(void) const { return (T *)(GObject *)pointer_; }

private:
  Pointer<GObject> pointer_;
};

class Text : Object {
public:
  Text(IBusText *text) : Object(text) {}
  Text(const gchar *str) : Object(ibus_text_new_from_string(str)) {}

  Text(const std::string &str)
      : Object(ibus_text_new_from_string(str.c_str())) {}

  Text(gunichar ch) : Object(ibus_text_new_from_unichar(ch)) {}

  void appendAttribute(guint type, guint value, guint start, guint end) {
    ibus_text_append_attribute(get<IBusText>(), type, value, start, end);
  }

  const gchar *text(void) const { return get<IBusText>()->text; }

  operator IBusText *(void) const { return get<IBusText>(); }
};

class StaticText : public Text {
public:
  StaticText(const gchar *str) : Text(ibus_text_new_from_static_string(str)) {}

  StaticText(const std::string &str)
      : Text(ibus_text_new_from_static_string(str.c_str())) {}

  StaticText(gunichar ch) : Text(ch) {}

  operator IBusText *(void) const { return Text::operator IBusText *(); }
};

class LookupTable : Object {
public:
  LookupTable(guint page_size = 10, guint cursor_pos = 0,
              gboolean cursor_visible = TRUE, gboolean round = FALSE)
      : Object(ibus_lookup_table_new(page_size, cursor_pos, cursor_visible,
                                     round)) {}

  guint pageSize(void) { return ibus_lookup_table_get_page_size(*this); }
  guint orientation(void) { return ibus_lookup_table_get_orientation(*this); }
  guint cursorPos(void) { return ibus_lookup_table_get_cursor_pos(*this); }
  guint size(void) { return ibus_lookup_table_get_number_of_candidates(*this); }

  gboolean pageUp(void) { return ibus_lookup_table_page_up(*this); }
  gboolean pageDown(void) { return ibus_lookup_table_page_down(*this); }
  gboolean cursorUp(void) { return ibus_lookup_table_cursor_up(*this); }
  gboolean cursorDown(void) { return ibus_lookup_table_cursor_down(*this); }

  void setPageSize(guint size) { ibus_lookup_table_set_page_size(*this, size); }
  void setCursorPos(guint pos) { ibus_lookup_table_set_cursor_pos(*this, pos); }
  void setOrientation(gint orientation) {
    ibus_lookup_table_set_orientation(*this, orientation);
  }
  void clear(void) { ibus_lookup_table_clear(*this); }
  void setCursorVisable(gboolean visable) {
    ibus_lookup_table_set_cursor_visible(*this, visable);
  }
  void setLabel(guint index, IBusText *text) {
    ibus_lookup_table_set_label(*this, index, text);
  }
  void appendCandidate(IBusText *text) {
    ibus_lookup_table_append_candidate(*this, text);
  }
  void appendLabel(IBusText *text) {
    ibus_lookup_table_append_label(*this, text);
  }
  IBusText *getCandidate(guint index) {
    return ibus_lookup_table_get_candidate(*this, index);
  }

  operator IBusLookupTable *(void) const { return get<IBusLookupTable>(); }
};

class Property : public Object {
public:
  Property(const gchar *key, IBusPropType type = PROP_TYPE_NORMAL,
           IBusText *label = NULL, const gchar *icon = NULL,
           IBusText *tooltip = NULL, gboolean sensitive = TRUE,
           gboolean visible = TRUE, IBusPropState state = PROP_STATE_UNCHECKED,
           IBusPropList *props = NULL)
      : Object(ibus_property_new(key, type, label, icon, tooltip, sensitive,
                                 visible, state, props)) {}

  void setLabel(IBusText *text) {
    ibus_property_set_label(get<IBusProperty>(), text);
  }

  void setLabel(const gchar *text) { setLabel(Text(text)); }

  void setIcon(const gchar *icon) {
    ibus_property_set_icon(get<IBusProperty>(), icon);
  }

  void setSymbol(IBusText *text) {
    ibus_property_set_symbol(get<IBusProperty>(), text);
  }

  void setSymbol(const gchar *text) { setSymbol(Text(text)); }

  void setSensitive(gboolean sensitive) {
    ibus_property_set_sensitive(get<IBusProperty>(), sensitive);
  }

  void setTooltip(IBusText *text) {
    ibus_property_set_tooltip(get<IBusProperty>(), text);
  }

  void setTooltip(const gchar *text) { setTooltip(Text(text)); }

  operator IBusProperty *(void) const { return get<IBusProperty>(); }
};

class PropList : Object {
public:
  PropList(void) : Object(ibus_prop_list_new()) {}

  void append(Property &prop) {
    ibus_prop_list_append(get<IBusPropList>(), prop);
  }

  operator IBusPropList *(void) const { return get<IBusPropList>(); }
};

} // namespace g
