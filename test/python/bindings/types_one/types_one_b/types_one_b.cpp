#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

namespace types_one
{
namespace types_one_b
{

template <typename T>
class RValueReference
{
public:
    static RValueReference<T> new_copied(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_copied()};
    }
    static RValueReference<T> new_moved(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_moved()};
    }

    T&& get() { return std::move(*p_); }

private:
    RValueReference(T* p) : p_(p) {}
    std::unique_ptr<T> p_;

public:
    REFLECT((RValueReference<T>),
            FIELDS(),
            METHODS(/*get, getRvalue*/))
};

template <typename T>
RValueReference<T>
copy_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_copied(p);
}

template <typename T>
RValueReference<T>
move_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_moved(p);
}

}  // namespace types_one_b
}  // namespace types_one


REFLECT_LIB_PYTHON_MODULE(types_one__types_one_b, module)
{
    reflect_lib::Module m(module);

	m.bind<types_one::types_one_b::Derived3<int, int>>();
    m.bind<types_one::types_one_b::Derived3<int, double>>();

    // import necessary because of cross-module inheritance
    pybind11::module::import("types_one.types_one_a");
    m.bind<types_one::types_one_b::NoCopyDerived>();

    m.bind<types_one::types_one_b::RValueReference<
        std::unique_ptr<types_one::Base>>>();
    m.bind<types_one::types_one_b::RValueReference<
        std::unique_ptr<types_one::types_one_a::NoCopy>>>();

    m.module.def(
        "move_to_rvalue_reference",
        &types_one::types_one_b::move_to_rvalue_reference<types_one::Base>);
    m.module.def("move_to_rvalue_reference",
                 &types_one::types_one_b::move_to_rvalue_reference<
                     types_one::types_one_a::NoCopy>);
}
