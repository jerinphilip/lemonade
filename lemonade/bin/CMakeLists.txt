add_executable(lemonade-bin main.cpp )
target_link_libraries(lemonade-bin PRIVATE lemonade)
target_include_directories(lemonade-bin PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})
set_target_properties(lemonade-bin PROPERTIES OUTPUT_NAME lemonade)

add_executable(lemonade-driver driver.cpp )
target_link_libraries(lemonade-driver PRIVATE lemonade)
target_include_directories(lemonade-driver PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})

add_executable(lemonade-server server.cpp)
target_link_libraries(lemonade-server PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-server PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})

add_executable(lemonade-client client.cpp )
target_link_libraries(lemonade-client PRIVATE lemonade simple-websocket-server)
target_include_directories(lemonade-client PRIVATE ${LEMONADE_INCLUDE_DIRECTORIES})
