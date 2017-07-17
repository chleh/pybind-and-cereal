#include "reflect-lib/pybind.h"

#include "test/test_types.h"

PYBIND11_MODULE(bindings, m) {
    using namespace reflect_lib;

    bind_with_pybind<Base>(m);
    bind_with_pybind<Derived1>(m);
    bind_with_pybind<Derived2>(m);
    bind_with_pybind<NoCopy>(m);

    bind_with_pybind<Derived3<int, int>>(m);
    bind_with_pybind<Derived3<int, double>>(m);

    bind_with_pybind<VectorTest>(m);
}

