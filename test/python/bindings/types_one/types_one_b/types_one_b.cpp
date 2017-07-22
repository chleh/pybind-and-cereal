#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_b, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::types_one_b::Derived3<int, int>>();
    m.bind<types_one::types_one_b::Derived3<int, double>>();
}
