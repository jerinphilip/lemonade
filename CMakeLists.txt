cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeDependentOption)

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif ()

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(lemonade CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(COMPILE_CUDA OFF CACHE BOOL "Compile GPU version")
SET(COMPILE_WASM OFF CACHE BOOL "Don't do wasm whatever this is")

SET(USE_SENTENCEPIECE ON CACHE BOOL "Download and compile SentencePiece")
SET(USE_STATIC_LIBS ON CACHE BOOL "Link statically against non-system libs")
SET(USE_WASM_COMPATIBLE_SOURCE OFF CACHE BOOL "Don't build wasm compatible sources")

find_package(Threads REQUIRED) # Cross-platform compatible way to get threads?
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Network DBus REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui PrintSupport Widgets LinguistTools Network DBus Svg REQUIRED)


include(GetVersionFromFile)
message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION_STRING_FULL}")


set(AUTHOR "Jerin Philip <jerinphilip@live.in>")
set(AUTHOR_XML_ESCAPED "Jerin Philip &lt;jerinphilip@live.in&gt;")

set(PROJECT_SHORTNAME "lemonade")
set(PROJECT_LONGNAME "Lemonade")
set(PROJECT_DESCRIPTION "Lemonade provides client-side translation on the local *nix machine.")
set(PROJECT_HOMEPAGE "https://github.com/jerinphilip/lemonade")
set(PROJECT_LICENSE "GPL")

set(LEMONADE_INCLUDE_DIRECTORIES 
    ${CMAKE_SOURCE_DIR}/3rd_party/CLI
    ${CMAKE_SOURCE_DIR}/3rd_party/rapidjson/include
    ${CMAKE_SOURCE_DIR}
)

set(LINK_LIBRARIES
    bergamot-translator
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::DBus
    ${LibArchive_LIBRARIES}
    ${CMAKE_DL_LIBS} # On unix necessary sometimes
)

if(UNIX)
include(GNUInstallDirs)

add_subdirectory(3rd_party)
add_subdirectory(lemonade)
add_subdirectory(bindings)

install(TARGETS lemonade-bin lemonade-server lemonade-client RUNTIME)
install(TARGETS ibus-engine-lemonade RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
install(FILES ${CMAKE_BINARY_DIR}/lemonade.xml DESTINATION /usr/share/ibus/component)
 # install(FILES ${CMAKE_SOURCE_DIR}/logo/lemonade_logo.png DESTINATION share/icons)
 # install(FILES ${CMAKE_SOURCE_DIR}/logo/lemonade_logo.svg DESTINATION share/icons)
endif(UNIX)

