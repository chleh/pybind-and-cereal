set(REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX "REFLECT_LIB_INTERNAL__" CACHE STRING
    "name prefix for generated python modules")

function(reflect_lib_split_python_module_package full_module_name)
    #string(REGEX REPLACE "(.*[.])[^.]*" "\\1" packages ${full_module_name})
    #string(REPLACE "." "__" packages_mangled ${packages})
    #string(REGEX REPLACE "(.*[.])([^.]*)" "\\2" module_name ${full_module_name})
    set(packages ${full_module_name})
    string(REPLACE "." "__" packages_mangled ${packages})

    set(REFLECT_LIB_PYTHON_MODULE_NAME
        "${REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX}${packages_mangled}"
        PARENT_SCOPE)
    set(REFLECT_LIB_PYTHON_FULL_MODULE_NAME
        "${packages}.${REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX}${packages_mangled}"
        PARENT_SCOPE)
endfunction()

function(reflect_lib_add_python_module full_module_name)
    reflect_lib_split_python_module_package(${full_module_name})

    # message("pybind11_add_module(${REFLECT_LIB_PYTHON_MODULE_NAME} ${ARGN})")
    pybind11_add_module("${REFLECT_LIB_PYTHON_MODULE_NAME}" ${ARGN})
    target_compile_definitions("${REFLECT_LIB_PYTHON_MODULE_NAME}" PRIVATE
        "-DREFLECT_LIB_PYTHON_MODULE_NAME_PREFIX=${REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX}")
    configure_file("${REFLECT_LIB_SOURCE_ROOT}/cmake/__init__.py.in"
        "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
endfunction()

function(reflect_lib_target_link_libraries full_module_name)
    reflect_lib_split_python_module_package(${full_module_name})

    message(${REFLECT_LIB_PYTHON_MODULE_NAME})
    target_link_libraries(${REFLECT_LIB_PYTHON_MODULE_NAME} ${ARGN})
endfunction()
