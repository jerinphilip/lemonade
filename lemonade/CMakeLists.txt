set(IBUS_BUS_NAME "org.freedesktop.IBus.Lemonade")
set(IBUS_COMMANDLINE "${CMAKE_INSTALL_LIBEXECDIR}/ibus-engine-lemonade")
set(IBUS_TEXTDOMAIN "ibus-lemonade")
set(IBUS_ICON "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/bergamot.png")
set(IBUS_LAYOUT "us")
set(IBUS_LANGUAGE "en")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lemonade.xml.in
               ${CMAKE_BINARY_DIR}/lemonade.xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ibus_config.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/ibus_config.h @ONLY)

add_library(lemonade STATIC engine_compat.cpp lemonade_engine.cpp
                            translator.cpp logging.cpp application.cpp)
target_link_libraries(lemonade PUBLIC ${LEMONADE_PRIVATE_LIBS})

target_include_directories(
  lemonade PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

target_include_directories(lemonade PUBLIC ${GLIB2_INCLUDE_DIRS}
                                           ${IBUS_INCLUDE_DIRS})

add_executable(ibus-engine-lemonade main.cpp)
target_link_libraries(ibus-engine-lemonade PUBLIC lemonade)

add_executable(test test.cpp)
target_link_libraries(test PUBLIC lemonade)
