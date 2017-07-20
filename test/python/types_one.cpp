#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"
#include "test/types/types_one/types_one_b/types_one_b.h"

PYBIND11_MODULE(types_one, m) {
    using namespace reflect_lib;

    bind_with_pybind<types_one::Base>(m);
    bind_with_pybind<types_one::types_one_a::types_one_a_a::Derived1>(m);
    bind_with_pybind<types_one::types_one_a::types_one_a_a::Derived2>(m);
    bind_with_pybind<types_one::types_one_a::NoCopy>(m);

    bind_with_pybind<types_one::types_one_b::Derived3<int, int>>(m);
    bind_with_pybind<types_one::types_one_b::Derived3<int, double>>(m);

    bind_with_pybind<types_one::VectorTest>(m);
}
