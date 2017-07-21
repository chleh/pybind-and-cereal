#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"
#include "test/types/types_one/types_one_b/types_one_b.h"

PYBIND11_MODULE(types_one, module) {
    reflect_lib::Module m(module);

    m.bind<types_one::Base>();
    m.bind<types_one::types_one_a::types_one_a_a::Derived1>();
    m.bind<types_one::types_one_a::types_one_a_a::Derived2>();
    m.bind<types_one::types_one_a::NoCopy>();

    m.bind<types_one::types_one_b::Derived3<int, int>>();
    m.bind<types_one::types_one_b::Derived3<int, double>>();

    m.bind<types_one::VectorTest>();
}
