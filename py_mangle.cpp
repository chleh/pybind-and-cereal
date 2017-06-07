#include <pybind11/pybind11.h>

#include "reflect-lib/remangle.h"

PYBIND11_MODULE(py_mangle, m) {
    m.def("demangle2", demangle2);
}

