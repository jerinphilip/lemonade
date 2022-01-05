
add_subdirectory(lib)
add_subdirectory(bin)

if(NOT APPLE)
  add_subdirectory(ibus)
endif(NOT APPLE)
