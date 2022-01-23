add_subdirectory(bergamot-translator EXCLUDE_FROM_ALL)

if(COMPILE_PYTHON)
    add_subdirectory(pybind11 EXCLUDE_FROM_ALL)
else(COMPILE_PYTHON)
    add_subdirectory(Simple-WebSocket-Server EXCLUDE_FROM_ALL)
    add_subdirectory(crow EXCLUDE_FROM_ALL)
endif(COMPILE_PYTHON)
