cmake_minimum_required(VERSION 3.1)
project(pybind_example)

set (CMAKE_CXX_STANDARD 14)
ADD_DEFINITIONS("-Wall")

FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)

add_subdirectory(pybind11)

set(PYBIND11_PYTHON_VERSION 3)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
#   "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# 
# message("CMP: ${CMAKE_MODULE_PATH}")
# find_package(Pybind11)

# add_executable(main main.cpp)
# target_link_libraries(main ${PYTHON_LIBRARIES})
# target_include_directories(main PRIVATE "pybind/include" ${PYTHON_INCLUDE_DIRS})
# 
# add_executable(main2 main2.cpp custom_struct_bind.cpp)
# target_link_libraries(main2 ${PYTHON_LIBRARIES})
# target_include_directories(main2 PRIVATE "pybind/include" ${PYTHON_INCLUDE_DIRS})

# pybind11_add_module(test_pybind SHARED test_pybind.cpp)
pybind11_add_module(test_pybind test_pybind.cpp)
