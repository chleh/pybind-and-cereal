#include "part1.h"
#include "part2.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_a__types_one_a_a, module)
{
    reflect_lib::Module m(module);

    // bindings are split into two compilation units
    bind_Derived1(m);
    bind_Derived2(m);
}
