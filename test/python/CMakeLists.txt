
pybind11_add_module(bindings bindings.cpp)
target_link_libraries(bindings PRIVATE reflection)

add_test(NAME TestDerived1
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_Derived1.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
