
add_subdirectory(lib)
add_subdirectory(bin)

if(!APPLE)
  add_subdirectory(ibus)
endif(!APPLE)
