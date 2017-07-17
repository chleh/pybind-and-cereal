#include <pybind11/pybind11.h>

#include "reflect-lib/remangle.h"

PYBIND11_MODULE(py_mangle, m) {
    using namespace reflect_lib;

    m.def("demangle2", demangle2);
}

