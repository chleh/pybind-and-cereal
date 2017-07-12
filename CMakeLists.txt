cmake_minimum_required(VERSION 3.1)
project(pybind_cereal_example)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
enable_testing()

set (CMAKE_CXX_STANDARD 14)
ADD_DEFINITIONS("-Wall")
set(PYBIND11_PYTHON_VERSION 3)

add_subdirectory(pybind11)

add_subdirectory(reflect-lib)
add_subdirectory(aux)
add_subdirectory(test)

