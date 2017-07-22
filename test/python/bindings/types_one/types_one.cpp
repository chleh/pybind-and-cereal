#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one.h"

REFLECT_LIB_PYTHON_MODULE(types_one, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::Base>();
    m.bind<types_one::VectorTest>();
}
