#include "reflect-lib/pybind.h"

#include "test/types/aux_bindings/tuple.h"

REFLECT_LIB_PYTHON_MODULE(aux_bindings, module)
{
    using namespace aux_bindings;

    reflect_lib::Module m(module);

    m.bind<ContainsTuple>();
}
