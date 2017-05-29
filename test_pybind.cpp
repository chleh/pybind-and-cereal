#include "reflect-lib/pybind.h"

#include "test_types.h"


PYBIND11_MODULE(test_pybind, m) {
    bind_with_pybind<Base>(m);
    bind_with_pybind<Derived1>(m);
    bind_with_pybind<Derived2>(m);
}


