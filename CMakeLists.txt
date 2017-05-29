cmake_minimum_required(VERSION 3.1)
project(pybind_cereal_example)

set (CMAKE_CXX_STANDARD 14)
ADD_DEFINITIONS("-Wall")

# FIND_PACKAGE(PythonInterp)
# FIND_PACKAGE(PythonLibs)

add_subdirectory(pybind11)

set(PYBIND11_PYTHON_VERSION 3)

pybind11_add_module(test_pybind test_pybind.cpp)

add_executable(test_serialization test_serialization.cpp)
