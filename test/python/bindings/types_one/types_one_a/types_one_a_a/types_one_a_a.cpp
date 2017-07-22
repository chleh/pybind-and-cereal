#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_a__types_one_a_a, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::types_one_a::types_one_a_a::Derived1>();
    m.bind<types_one::types_one_a::types_one_a_a::Derived2>();
}
