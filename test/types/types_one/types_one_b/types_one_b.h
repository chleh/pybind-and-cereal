#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

#include "test/types/types_one/types_one.h"
#include "test/types/types_one/types_one_a/types_one_a.h"

namespace types_one
{
namespace types_one_b
{
template <typename T, typename U>
struct Derived3 : Base {
    T t;
    U u;
    std::vector<float> v;
    std::vector<float> w;

    std::string what() override { return "der3"; }

    // test unique_ptr methods
    // std::unique_ptr<Base> f(std::unique_ptr<int> const&) { return nullptr; }
    // TODO different type?
    std::unique_ptr<Base> f() { return std::make_unique<Derived3<T,U>>(); }

    // TODO doesn't work: "Holder classes are only supported for custom types"
    // --> maybe wrap into further class?
    std::unique_ptr<int> fi() { return std::make_unique<int>(); }

    std::unique_ptr<int> const& g() const
    {
        static auto p = std::make_unique<int>();
        return p;
    }

    // also check if aux types are derived from method arguments and return
    // types
    std::unique_ptr<std::vector<types_one_a::NoCopy>>& h()
    {
        static auto p = std::make_unique<std::vector<types_one_a::NoCopy>>();
        return p;
    }

    int get_int_from_unique_ptr(
        std::unique_ptr<types_one::types_one_a::NoCopy> const& p)
    {
        if (p) return 2* (p->i);
        return 0;
    }

    int nocopy_method_arg(types_one::types_one_a::NoCopy const& nc)
    {
        return nc.i;
    }

    REFLECT_DERIVED((Derived3<T, U>), (Base), FIELDS(t, u, v, w),
                    METHODS(f,
                            nocopy_method_arg))  // TODO g, h
};

struct NoCopyDerived : public types_one::types_one_a::NoCopy
{
    virtual std::string m() const override  { return "NoCopyDerived"; }

    REFLECT_DERIVED((NoCopyDerived), (types_one::types_one_a::NoCopy), FIELDS(),
                    METHODS())
};

}  // namespace types_one_b
}  // namespace types_one
