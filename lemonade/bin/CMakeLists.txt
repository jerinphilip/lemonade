add_executable(lemonade-bin EXCLUDE_FROM_ALL main.cpp)
target_link_libraries(lemonade-bin PRIVATE lemonade)
target_include_directories(lemonade-bin PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})
set_target_properties(lemonade-bin PROPERTIES OUTPUT_NAME lemonade)


add_executable(rest-server EXCLUDE_FROM_ALL rest-server.cpp)
target_link_libraries(rest-server PRIVATE lemonade Crow)
target_include_directories(rest-server PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})

add_executable(lemonade-server EXCLUDE_FROM_ALL server.cpp)
target_link_libraries(lemonade-server PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-server PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})

add_executable(lemonade-client EXCLUDE_FROM_ALL client.cpp)
target_link_libraries(lemonade-client PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-client PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})
