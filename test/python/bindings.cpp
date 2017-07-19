#include "reflect-lib/pybind.h"

#include "test/types/test_types.h"

PYBIND11_MODULE(bindings, m) {
    using namespace reflect_lib;

    bind_with_pybind<types_one::Base>(m);
    bind_with_pybind<types_one::Derived1>(m);
    bind_with_pybind<types_one::Derived2>(m);
    bind_with_pybind<types_one::NoCopy>(m);

    bind_with_pybind<types_one::Derived3<int, int>>(m);
    bind_with_pybind<types_one::Derived3<int, double>>(m);

    bind_with_pybind<types_one::VectorTest>(m);
}

