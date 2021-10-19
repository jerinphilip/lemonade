#include "engine_compat.h"
#include "lemonade_engine.h"
#include <cstring>

namespace lemonade::ibus {

/* code of engine class of GObject */
#define IBUS_LEMONADE_ENGINE(obj)                                              \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), IBUS_TYPE_LEMONADE_ENGINE,                \
                              IBusLemonadeEngine))
#define IBUS_LEMONADE_ENGINE_CLASS(klass)                                      \
  (G_TYPE_CHECK_CLASS_CAST((klass), IBUS_TYPE_LEMONADE_ENGINE,                 \
                           IBusLemonadeEngineClass))
#define IBUS_IS_LEMONADE_ENGINE(obj)                                           \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), IBUS_TYPE_LEMONADE_ENGINE))
#define IBUS_IS_LEMONADE_ENGINE_CLASS(klass)                                   \
  (G_TYPE_CHECK_CLASS_TYPE((klass), IBUS_TYPE_LEMONADE_ENGINE))
#define IBUS_LEMONADE_ENGINE_GET_CLASS(obj)                                    \
  (G_TYPE_INSTANCE_GET_CLASS((obj), IBUS_TYPE_LEMONADE_ENGINE,                 \
                             IBusLemonadeEngineClass))

typedef struct _IBusLemonadeEngine IBusLemonadeEngine;
typedef struct _IBusLemonadeEngineClass IBusLemonadeEngineClass;

struct _IBusLemonadeEngineClass {
  IBusEngineClass parent;
};

struct _IBusLemonadeEngine {
  IBusEngine parent;

  /* members */
  Engine *engine;
};

/* functions prototype */
static void ibus_lemonade_engine_class_init(IBusLemonadeEngineClass *klass);
static void ibus_lemonade_engine_init(IBusLemonadeEngine *lemonade);
static GObject *
ibus_lemonade_engine_constructor(GType type, guint n_construct_params,
                                 GObjectConstructParam *construct_params);

static void ibus_lemonade_engine_destroy(IBusLemonadeEngine *lemonade);
static gboolean ibus_lemonade_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers);
static void ibus_lemonade_engine_focus_in(IBusEngine *engine);
static void ibus_lemonade_engine_focus_out(IBusEngine *engine);
#if IBUS_CHECK_VERSION(1, 5, 4)
static void ibus_lemonade_engine_set_content_type(IBusEngine *engine,
                                                  guint purpose, guint hints);
#endif
static void ibus_lemonade_engine_reset(IBusEngine *engine);
static void ibus_lemonade_engine_enable(IBusEngine *engine);
static void ibus_lemonade_engine_disable(IBusEngine *engine);

#if 0
static void     ibus_engine_set_cursor_location (IBusEngine             *engine,
                                                 gint                    x,
                                                 gint                    y,
                                                 gint                    w,
                                                 gint                    h);
static void     ibus_lemonade_engine_set_capabilities
                                                (IBusEngine             *engine,
                                                 guint                   caps);
#endif

static void ibus_lemonade_engine_page_up(IBusEngine *engine);
static void ibus_lemonade_engine_page_down(IBusEngine *engine);
static void ibus_lemonade_engine_cursor_up(IBusEngine *engine);
static void ibus_lemonade_engine_cursor_down(IBusEngine *engine);
static void ibus_lemonade_engine_property_activate(IBusEngine *engine,
                                                   const gchar *prop_name,
                                                   guint prop_state);
static void ibus_lemonade_engine_candidate_clicked(IBusEngine *engine,
                                                   guint index, guint button,
                                                   guint state);
#if 0
static void ibus_lemonade_engine_property_show    (IBusEngine             *engine,
                                                 const gchar            *prop_name);
static void ibus_lemonade_engine_property_hide    (IBusEngine             *engine,
                                                 const gchar            *prop_name);
#endif

G_DEFINE_TYPE(IBusLemonadeEngine, ibus_lemonade_engine, IBUS_TYPE_ENGINE)

static void ibus_lemonade_engine_class_init(IBusLemonadeEngineClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS(klass);
  IBusEngineClass *engine_class = IBUS_ENGINE_CLASS(klass);

  object_class->constructor = ibus_lemonade_engine_constructor;
  ibus_object_class->destroy =
      (IBusObjectDestroyFunc)ibus_lemonade_engine_destroy;

  engine_class->process_key_event = ibus_lemonade_engine_process_key_event;

  engine_class->reset = ibus_lemonade_engine_reset;
  engine_class->enable = ibus_lemonade_engine_enable;
  engine_class->disable = ibus_lemonade_engine_disable;

  engine_class->focus_in = ibus_lemonade_engine_focus_in;
  engine_class->focus_out = ibus_lemonade_engine_focus_out;

#if IBUS_CHECK_VERSION(1, 5, 4)
  engine_class->set_content_type = ibus_lemonade_engine_set_content_type;
#endif

  engine_class->page_up = ibus_lemonade_engine_page_up;
  engine_class->page_down = ibus_lemonade_engine_page_down;

  engine_class->cursor_up = ibus_lemonade_engine_cursor_up;
  engine_class->cursor_down = ibus_lemonade_engine_cursor_down;

  engine_class->property_activate = ibus_lemonade_engine_property_activate;

  engine_class->candidate_clicked = ibus_lemonade_engine_candidate_clicked;
}

static void ibus_lemonade_engine_init(IBusLemonadeEngine *lemonade) {
  if (g_object_is_floating(lemonade))
    g_object_ref_sink(lemonade); // make engine sink
}

static GObject *
ibus_lemonade_engine_constructor(GType type, guint n_construct_params,
                                 GObjectConstructParam *construct_params) {
  IBusLemonadeEngine *engine;
  const gchar *name;

  engine =
      (IBusLemonadeEngine *)G_OBJECT_CLASS(ibus_lemonade_engine_parent_class)
          ->constructor(type, n_construct_params, construct_params);
  name = ibus_engine_get_name((IBusEngine *)engine);
  engine->engine = new LemonadeEngine(IBUS_ENGINE(engine));
  return (GObject *)engine;
}

static void ibus_lemonade_engine_destroy(IBusLemonadeEngine *lemonade) {
  delete lemonade->engine;
  ((IBusObjectClass *)ibus_lemonade_engine_parent_class)
      ->destroy((IBusObject *)lemonade);
}

static gboolean ibus_lemonade_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers) {
  IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;
  return lemonade->engine->processKeyEvent(keyval, keycode, modifiers);
}

#if IBUS_CHECK_VERSION(1, 5, 4)
static void ibus_lemonade_engine_set_content_type(IBusEngine *engine,
                                                  guint purpose, guint hints) {
  IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;
  return lemonade->engine->setContentType(purpose, hints);
}
#endif

static void ibus_lemonade_engine_property_activate(IBusEngine *engine,
                                                   const gchar *prop_name,
                                                   guint prop_state) {
  IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;
  lemonade->engine->propertyActivate(prop_name, prop_state);
}
static void ibus_lemonade_engine_candidate_clicked(IBusEngine *engine,
                                                   guint index, guint button,
                                                   guint state) {
  IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;
  lemonade->engine->candidateClicked(index, button, state);
}

#define FUNCTION(name, Name)                                                   \
  static void ibus_lemonade_engine_##name(IBusEngine *engine) {                \
    IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;               \
    lemonade->engine->Name();                                                  \
    ((IBusEngineClass *)ibus_lemonade_engine_parent_class)->name(engine);      \
  }
FUNCTION(focus_in, focusIn)
FUNCTION(focus_out, focusOut)
FUNCTION(reset, reset)
FUNCTION(enable, enable)
FUNCTION(disable, disable)
FUNCTION(page_up, pageUp)
FUNCTION(page_down, pageDown)
FUNCTION(cursor_up, cursorUp)
FUNCTION(cursor_down, cursorDown)
#undef FUNCTION

Engine::Engine(IBusEngine *engine) : m_engine(engine) {
#if IBUS_CHECK_VERSION(1, 5, 4)
  m_input_purpose = IBUS_INPUT_PURPOSE_FREE_FORM;
#endif
}

gboolean Engine::contentIsPassword() {
#if IBUS_CHECK_VERSION(1, 5, 4)
  return IBUS_INPUT_PURPOSE_PASSWORD == m_input_purpose;
#else
  return FALSE;
#endif
}

void Engine::focusOut(void) {
#if IBUS_CHECK_VERSION(1, 5, 4)
  m_input_purpose = IBUS_INPUT_PURPOSE_FREE_FORM;
#endif
}

#if IBUS_CHECK_VERSION(1, 5, 4)
void Engine::setContentType(guint purpose, guint hints) {
  m_input_purpose = (IBusInputPurpose)purpose;
}
#endif

Engine::~Engine(void) {}

} // namespace lemonade::ibus
