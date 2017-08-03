#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_b, module)
{
    reflect_lib::Module m(module);

    // pass std::unique_ptr via smart_ptr
    m.bind<types_one::types_one_b::Derived3<int, int>>().def(
        "get_int_from_unique_ptr",
        [](types_one::types_one_b::Derived3<int, int>& inst,
           reflect_lib::smart_ptr<types_one::types_one_a::NoCopy> const& p) {
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.get());
            // TODO: problematic: additional copy
            auto res = inst.get_int_from_unique_ptr(p_);
            p_.release();
            return res;
        });

    m.bind<types_one::types_one_b::Derived3<int, double>>();

    // import necessary because of cross-module inheritance
    pybind11::module::import("types_one.types_one_a");
    m.bind<types_one::types_one_b::NoCopyDerived>();
}
