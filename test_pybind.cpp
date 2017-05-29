#include "reflect-lib/pybind.h"

#include "test_types.h"


PYBIND11_MODULE(test_pybind, m) {
    wrap_into_pybind<Base>(m);

    // return m.ptr();
}


