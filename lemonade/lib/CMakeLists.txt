set(LIBRARY_SOURCES
   logging.cpp
   model_inventory.cpp
   translation_server.cpp
   translation_client.cpp
   translator.cpp
   utils.cpp
)

add_library(lemonade STATIC ${LIBRARY_SOURCES})
target_link_libraries(lemonade PUBLIC bergamot-translator marian ${CMAKE_DL_LIBS})
target_include_directories(lemonade PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})

# For IWYU
set_property(TARGET lemonade PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

