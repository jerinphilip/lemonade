cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeDependentOption)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif()
project(lemonade CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GetVersionFromFile)
message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION_STRING_FULL}")

add_subdirectory(3rd_party)

set(AUTHOR "Jerin Philip <jerinphilip@live.in>")
set(AUTHOR_XML_ESCAPED "Jerin Philip &lt;jerinphilip@live.in&gt;")
set(PROJECT_SHORTNAME "lemonade")
set(PROJECT_LONGNAME "Lemonade")
set(PROJECT_DESCRIPTION
    "Lemonade provides client-side translation on the local *nix machine.")
set(PROJECT_VERSION ${PROJECT_VERSION_STRING})
set(PROJECT_HOMEPAGE "https://github.com/jerinphilip/lemonade")
set(PROJECT_LICENSE "GPL")

set(LEMONADE_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/3rd_party/CLI
    ${CMAKE_SOURCE_DIR}/3rd_party/rapidjson/include ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR})

find_package(Threads REQUIRED) # Cross-platform compatible way to get threads?
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core
  REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
  REQUIRED)
find_package(OpenSSL REQUIRED)

set(LINK_LIBRARIES slimt Qt${QT_VERSION_MAJOR}::Core yaml-cpp::yaml-cpp)

include(GNUInstallDirs)
add_subdirectory(lemonade)
install(TARGETS ibus-engine-lemonade
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
install(FILES ${CMAKE_BINARY_DIR}/lemonade.xml
        DESTINATION /usr/share/ibus/component)
install(FILES ${CMAKE_SOURCE_DIR}/assets/bergamot.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
