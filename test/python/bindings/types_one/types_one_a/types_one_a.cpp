#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_a/types_one_a.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_a, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::types_one_a::NoCopy>();
}
