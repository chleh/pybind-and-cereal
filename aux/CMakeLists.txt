
pybind11_add_module(py_mangle py_mangle.cpp NO_EXTRAS)
target_link_libraries(py_mangle reflection)

