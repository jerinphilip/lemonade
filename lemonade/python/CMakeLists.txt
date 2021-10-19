
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG)

message("Using Python: " ${Python_EXECUTABLE})

# pybind11 method:
pybind11_add_module(pybergamot MODULE bergamot.cpp)
target_link_libraries(pybergamot PUBLIC pybind11::module bergamot-translator)
target_include_directories(pybergamot PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
set_target_properties(pybergamot PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}"
)

