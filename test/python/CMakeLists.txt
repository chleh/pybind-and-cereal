
pybind11_add_module(bindings bindings.cpp NO_EXTRAS)
target_link_libraries(bindings reflection)

