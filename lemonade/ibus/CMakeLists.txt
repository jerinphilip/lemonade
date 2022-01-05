
set(IBUS_BUS_NAME "org.freedesktop.IBus.Lemonade")
set(IBUS_COMMANDLINE "")
set(IBUS_TEXTDOMAIN "lemonade-ime")
set(IBUS_ICON "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/bergamot.png")
set(IBUS_LAYOUT "us")
set(IBUS_LANGUAGE "en")


find_package(PkgConfig)
PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lemonade.xml.in
               ${CMAKE_BINARY_DIR}/lemonade.xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/application.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/application.h @ONLY)


## Create your Executable and link it. `.....` your sources
add_executable(ibus-engine-lemonade main.cpp engine_compat.cpp lemonade_engine.cpp)
## `....` place your created libraries
target_link_libraries(ibus-engine-lemonade PUBLIC ${GLIB2_LIBRARIES} ${IBUS_LIBRARIES} lemonade simple-websocket-server)
target_include_directories(ibus-engine-lemonade PRIVATE ${GLIB2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS} ${LEMONADE_INCLUDE_DIRECTORIES})

