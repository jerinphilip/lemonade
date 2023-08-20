#include "application.h"
#include "logging.h"

namespace lemonade {

Application::Application(gboolean ibus) {
  // FIXME
  std::string log_path = std::getenv("HOME");
  log_path += "/.ibus-engine-lemonade.log";
  g_log("ibus-lemonade", G_LOG_LEVEL_MESSAGE, "Creating log at: %s",
        log_path.c_str());
  setup_logging(log_path);

  ibus_init();

  // TODO: Bus can be g::Object derived.
  bus_ = ibus_bus_new();

  if (!ibus_bus_is_connected(bus_)) {
    LOG("Cannot connect to ibus!");
    g_warning("Can not connect to ibus!");
    std::abort();
  }

  if (!ibus_bus_get_config(bus_)) {
    LOG("IBus config component is not ready!");
    g_warning("IBus config component is not ready!");
    std::abort();
  }

  g_object_ref_sink(bus_);

  auto callback = +[](IBusBus *bus_, gpointer user_data) { ibus_quit(); };
  g_signal_connect(bus_, "disconnected", G_CALLBACK(callback), NULL);

  LOG("Adding factory");
  factory_ = ibus_factory_new(ibus_bus_get_connection(bus_));

  ibus_factory_add_engine(factory_, PROJECT_SHORTNAME,
                          IBUS_TYPE_LEMONADE_ENGINE);

  if (ibus) {
    LOG("ibus = true, requesting bus");
    ibus_bus_request_name(bus_, IBUS_BUS_NAME, 0);
  } else {
    LOG("ibus = false, creating new bus");
    g::SharedPointer<IBusComponent> component = //
        ibus_component_new(                     //
            IBUS_BUS_NAME,                      //
            PROJECT_DESCRIPTION,                //
            PROJECT_VERSION,                    //
            PROJECT_LICENSE,                    //
            AUTHOR,                             //
            PROJECT_HOMEPAGE,                   //
            IBUS_COMPONENT_COMMANDLINE,         //
            IBUS_TEXTDOMAIN                     //
        );

    if (component) {
      LOG("creating component success");
    }

    g::SharedPointer<IBusEngineDesc> description = //
        ibus_engine_desc_new(                      //
            PROJECT_SHORTNAME,                     //
            PROJECT_LONGNAME,                      //
            PROJECT_DESCRIPTION,                   //
            IBUS_LANGUAGE,                         //
            PROJECT_LICENSE,                       //
            AUTHOR,                                //
            IBUS_ICON,                             //
            IBUS_LAYOUT                            //
        );

    ibus_component_add_engine(component, description);
    ibus_bus_register_component(bus_, component);
  }
}

void Application::run() {
  LOG("Spawning ibus main");
  ibus_main();
  LOG("Ending ibus main");
}
} // namespace lemonade
