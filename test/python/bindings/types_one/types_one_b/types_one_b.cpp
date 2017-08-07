#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

namespace types_one
{
namespace types_one_b
{


template <typename T>
class UniquePtrWrapper
{
public:
    UniquePtrWrapper(reflect_lib::smart_ptr<T>& p) : p_(p.new_copied()) {}

    std::unique_ptr<T>&& getR() { return std::move(p_); }

private:
    std::unique_ptr<T> p_;

public:
    REFLECT((UniquePtrWrapper<T>), FIELDS(), METHODS(getR))
};

template <typename T>
UniquePtrWrapper<T>
wrap(reflect_lib::smart_ptr<T>& p)
{
    std::cout << "use count: " << p.use_count() << '\n';
    return UniquePtrWrapper<T>(p);
}

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
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.get());
            // TODO: problematic: additional copy
            auto res = inst.get_int_from_unique_ptr(p_);
            p_.release();
            return res;
        });

    m.module.def(
        "i_up_cr",
        [](reflect_lib::smart_ptr<types_one::types_one_a::NoCopy> const& p) {
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.get());
            auto const& pr = p_;
            // TODO: problematic: additional copy
            auto res = types_one::types_one_b::i_up_cr(pr);
            p_.release();
            return res;
        });

    m.module.def(
        "i_up_r",
        [](reflect_lib::smart_ptr<types_one::types_one_a::NoCopy>& p) {
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.get());
            // TODO: problematic: additional copy
            auto res = types_one::types_one_b::i_up_r(p_);
            p_.release();
            return res;
        });

    m.module.def(
        "i_up_rr",
        [](reflect_lib::smart_ptr<types_one::types_one_a::NoCopy>& p) {
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.new_moved());
            return types_one::types_one_b::i_up_rr(std::move(p_));
        });

    m.module.def(
        "i_up",
        [](reflect_lib::smart_ptr<types_one::types_one_a::NoCopy>& p) {
            auto p_ = std::unique_ptr<types_one::types_one_a::NoCopy>(p.new_moved());
            return types_one::types_one_b::i_up(std::move(p_));
        });



    m.bind<types_one::types_one_b::Derived3<int, double>>();

    // import necessary because of cross-module inheritance
    pybind11::module::import("types_one.types_one_a");
    m.bind<types_one::types_one_b::NoCopyDerived>();

    m.bind<types_one::types_one_b::UniquePtrWrapper<types_one::Base>>();
    m.module.def("f", [](types_one::types_one_b::UniquePtrWrapper<types_one::Base>& p) {
        return types_one::types_one_b::f(p.getR());
    });
#if 0
    m.module.def("wrap", [](reflect_lib::smart_ptr<types_one::Base>& p) {

    });
#endif
    m.module.def("wrap", &types_one::types_one_b::wrap<types_one::Base>);
}
