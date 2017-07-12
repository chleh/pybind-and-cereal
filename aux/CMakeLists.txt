
pybind11_add_module(py_mangle py_mangle.cpp)
target_link_libraries(py_mangle PRIVATE reflection)
