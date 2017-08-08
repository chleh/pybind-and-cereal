#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"


template <typename T>
struct NoCleanup
{
    explicit NoCleanup(T* p_) : p{p_} {}
    std::unique_ptr<T> p;

    ~NoCleanup() { p.release(); }
};


namespace types_one
{
namespace types_one_b
{
template <typename T>
class UniquePtrReference
{
public:
    static UniquePtrReference<T> new_copied(reflect_lib::smart_ptr<T> const& p)
    {
        return UniquePtrReference<T>{p.new_copied()};
    }
    static UniquePtrReference<T> new_moved(reflect_lib::smart_ptr<T> const& p)
    {
        return UniquePtrReference<T>{p.new_moved()};
    }

    std::unique_ptr<T>& get() { return p_; }
    std::unique_ptr<T> && getRvalue() { return std::move(p_); }

private:
    UniquePtrReference(T* p) : p_(p) {}
    std::unique_ptr<T> p_;

public:
    REFLECT((UniquePtrReference<T>),
            FIELDS(),
            METHODS(/*get, getRvalue*/))
};

template <typename T>
UniquePtrReference<T>
copy_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    std::cout << "use count: " << p.use_count() << '\n';
    return UniquePtrReference<T>::new_copied(p);
}

template <typename T>
UniquePtrReference<T>
move_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    std::cout << "use count: " << p.use_count() << '\n';
    return UniquePtrReference<T>::new_moved(p);
}

// TODO unique_ptr const l-value-reference? --> bad style, not supported!




int f(std::unique_ptr<types_one::Base>&& p)
{
    std::cout << "p->i " << p->i << '\n';
    return p->i;
}



int i_up_cr(
    std::unique_ptr<types_one::types_one_a::NoCopy> const& p)
{
    if (p) return 2* (p->i);
    return 0;
}

int i_up_r(
    std::unique_ptr<types_one::types_one_a::NoCopy>& p)
{
    if (p) return 2* (p->i);
    return 0;
}

int i_up_rr(
    std::unique_ptr<types_one::types_one_a::NoCopy>&& p)
{
    if (p) return 2* (p->i);
    return 0;
}

int i_up(
    std::unique_ptr<types_one::types_one_a::NoCopy> p)
{
    if (p) return 2* (p->i);
    return 0;
}

}  // namespace types_one_b
}  // namespace types_one


REFLECT_LIB_PYTHON_MODULE(types_one__types_one_b, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::types_one_b::Derived3<int, int>>().def(
        "get_int_from_unique_ptr",
        [](types_one::types_one_b::Derived3<int, int>& inst,
           reflect_lib::smart_ptr<types_one::types_one_a::NoCopy> const& p) {
            NoCleanup<types_one::types_one_a::NoCopy> p_(p.get());
            return inst.get_int_from_unique_ptr(p_.p);
        });

    // unique_ptr specific, u.p. const& --> bad style!
    m.module.def(
        "i_up_cr",
        [](reflect_lib::smart_ptr<types_one::types_one_a::NoCopy> const& p) {
            NoCleanup<types_one::types_one_a::NoCopy> p_(p.get());
            auto const& pr = p_.p;
            return types_one::types_one_b::i_up_cr(pr);
        });

    // unique_ptr specific
    m.module.def(
        "i_up_r",
        [](types_one::types_one_b::UniquePtrReference<types_one::types_one_a::NoCopy>& p) {
            return types_one::types_one_b::i_up_r(p.get());
        });

    // for all && args?
    m.module.def(
        "i_up_rr",
        [](types_one::types_one_b::UniquePtrReference<types_one::types_one_a::NoCopy>& p) {
            return types_one::types_one_b::i_up_rr(p.getRvalue());
        });

    // for all non-copyable types
    m.module.def(
        "i_up",
        [](types_one::types_one_b::UniquePtrReference<types_one::types_one_a::NoCopy>& p) {
            return types_one::types_one_b::i_up(p.getRvalue());
        });

    // Does not work:
#if 0
    m.module.def(
        "i_up_cr2",
        [](std::unique_ptr<types_one::types_one_a::NoCopy> const& p) {
            return types_one::types_one_b::i_up_cr(p);
        });
#endif



    m.bind<types_one::types_one_b::Derived3<int, double>>();

    // import necessary because of cross-module inheritance
    pybind11::module::import("types_one.types_one_a");
    m.bind<types_one::types_one_b::NoCopyDerived>();

    m.bind<types_one::types_one_b::UniquePtrReference<types_one::Base>>();
    m.bind<types_one::types_one_b::UniquePtrReference<types_one::types_one_a::NoCopy>>();

    m.module.def("f", [](types_one::types_one_b::UniquePtrReference<types_one::Base>& p) {
        return types_one::types_one_b::f(p.getRvalue());
    });

    m.module.def("copy_to_unique_ptr", &types_one::types_one_b::copy_to_unique_ptr<types_one::Base>);
    m.module.def("move_to_unique_ptr", &types_one::types_one_b::move_to_unique_ptr<types_one::Base>);
    m.module.def("move_to_unique_ptr", &types_one::types_one_b::move_to_unique_ptr<types_one::types_one_a::NoCopy>);
}
