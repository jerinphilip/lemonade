#pragma once
#include "engine_compat.h"
#include "ibus_config.h"
#include "logging.h"
#include <ibus.h>

static g::SharedPointer<IBusFactory> factory = nullptr;

namespace lemonade {
class Application {
public:
  Application(gboolean ibus) {
    // FIXME
    std::string log_path =
        fmt::format("{}/.ibus-engine-lemonade.log", std::getenv("HOME"));
    setupLogging(log_path);

    ibus_init();

    // TODO: Bus can be g::Object derived.
    bus = ibus_bus_new();

    if (!ibus_bus_is_connected(bus)) {
      getLogger()->info("Cannot connect to ibus!");
      g_warning("Can not connect to ibus!");
      std::abort();
    }

    if (!ibus_bus_get_config(bus)) {
      getLogger()->info("IBus config component is not ready!");
      g_warning("IBus config component is not ready!");
      std::abort();
    }

    g_object_ref_sink(bus);

    auto callback = +[](IBusBus *bus, gpointer user_data) { ibus_quit(); };
    g_signal_connect(bus, "disconnected", G_CALLBACK(callback), NULL);

    getLogger()->info("Adding factory");
    factory = ibus_factory_new(ibus_bus_get_connection(bus));

    ibus_factory_add_engine(factory, PROJECT_SHORTNAME,
                            IBUS_TYPE_LEMONADE_ENGINE);

    if (ibus) {
      getLogger()->info("ibus = true, requesting bus");
      ibus_bus_request_name(bus, IBUS_BUS_NAME, 0);
    } else {
      getLogger()->info("ibus = false, creating new bus");
      g::SharedPointer<IBusComponent> component;

      component = ibus_component_new(IBUS_BUS_NAME,              //
                                     PROJECT_DESCRIPTION,        //
                                     PROJECT_VERSION,            //
                                     PROJECT_LICENSE,            //
                                     AUTHOR,                     //
                                     PROJECT_HOMEPAGE,           //
                                     IBUS_COMPONENT_COMMANDLINE, //
                                     IBUS_TEXTDOMAIN);

      if (component) {
        getLogger()->info("creating component success");
      }

      ibus_component_add_engine(component,
                                ibus_engine_desc_new(PROJECT_SHORTNAME,   //
                                                     PROJECT_LONGNAME,    //
                                                     PROJECT_DESCRIPTION, //
                                                     IBUS_LANGUAGE,       //
                                                     PROJECT_LICENSE,     //
                                                     AUTHOR,              //
                                                     IBUS_ICON,           //
                                                     IBUS_LAYOUT));
      ibus_bus_register_component(bus, component);
    }
  }

  void run() {
    getLogger()->info("Spawning ibus main");
    ibus_main();
    getLogger()->info("Ending ibus main");
  }

private:
  IBusBus *bus = NULL;
};
} // namespace lemonade
