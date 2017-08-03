#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one.h"

namespace types_one
{
struct BaseTrampoline : public Base
{
    using Base::Base;

    std::string what() override {
        PYBIND11_OVERLOAD_PURE(
            std::string, /* Return type */
            Base,        /* Parent class */
            what         /* Name of function in C++ (must match Python name) */
            /* This function has no arguments */
            );
    }

    REFLECT_DERIVED((BaseTrampoline), (Base), FIELDS(), METHODS())
};

template<typename T>
class UniquePtrReference
{
public:
    UniquePtrReference(reflect_lib::smart_ptr<T> const& p) : p_(p) {}

    reflect_lib::smart_ptr<T> const& get() const { return p_; }
private:
    reflect_lib::smart_ptr<T> const& p_;

public:
    REFLECT((UniquePtrReference<T>), FIELDS(), METHODS())
};
/*
template<typename T>
class UniquePtrReference
{
public:
    UniquePtrReference(std::unique_ptr<T> const& p) : p_(p) {}

    std::unique_ptr<T> const& get() const { return p_; }
private:
    std::unique_ptr<T> const& p_;
};
*/

void f(std::unique_ptr<int> const& p) {
    std::cout << "p: " << p.get() << '\n';
}

void g(UniquePtrReference<types_one::Base> const& p) {
    std::cout << "p: " << p.get().get() << '\n';
}

}  // namespace types_one

REFLECT_LIB_PYTHON_MODULE(types_one, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::Base, types_one::BaseTrampoline>();

    m.module.def("say_what", types_one::say_what);

    m.bind<types_one::VectorTest>();

    m.bind<types_one::NonDefaultConstructible>();

    m.bind<types_one::UniquePtrReference<types_one::Base>>().def(
        pybind11::init<reflect_lib::smart_ptr<types_one::Base> const&>());

    pybind11::implicitly_convertible<
        reflect_lib::smart_ptr<types_one::Base>,
        // types_one::Base,
        types_one::UniquePtrReference<types_one::Base>>();
    // m.module.def("f", &f);
    m.module.def("g", &types_one::g);
}
