#include "reflect-lib/pybind.h"

#include "test/types/pickle_types/pickle_types.h"

REFLECT_LIB_PYTHON_MODULE(pickle_types, module)
{
    using namespace pickle_types;

    reflect_lib::Module m(module);

    m.bind<Empty>();
    m.bind<DerivedFromEmptyInt>();
    m.bind<DerivedFromEmptyString>();
    m.bind<OwnsEmpty>();
    m.bind<ContainsVectorOfEmpty>();
    m.bind<ContainsVectorOfIntString>();
}
