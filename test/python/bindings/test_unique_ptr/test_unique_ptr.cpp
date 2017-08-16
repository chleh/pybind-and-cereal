#include "reflect-lib/pybind.h"

#include "test/types/test_unique_ptr/test_unique_ptr.h"
#include "test/types/test_unique_ptr/test_rvalue_refs.h"

REFLECT_LIB_PYTHON_MODULE(test_unique_ptr, module)
{
    reflect_lib::Module m(module);
    m.bind<test_unique_ptr::UniquePtrTest>();
    m.bind<test_unique_ptr::RValueRefTest>();

    m.bind<test_unique_ptr::CtorTest>();
}
