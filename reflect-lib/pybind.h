#pragma once

#include <utility>

#include <pybind11/pybind11.h>

#include "reflect-macros.h"


struct NoOp
{
    template<typename... Ts>
    NoOp(Ts&&...)
    {
    }
};


template <typename Visitor, typename... Ts>
void visit(Visitor&& v, std::tuple<Ts...>&& t)
{
    using Idcs = std::index_sequence_for<Ts...>;
    visit_impl(std::forward<Visitor>(v), std::forward<std::tuple<Ts...>>(t), Idcs{});
}

template <typename Visitor, typename... Ts, std::size_t... Idcs>
void visit_impl(Visitor&& v, std::tuple<Ts...>&& t, std::index_sequence<Idcs...>)
{
    NoOp{ (v(std::forward<Ts>(std::get<Idcs>(t))), 0) ... };
}

// derived class
template<typename Class>
decltype(auto)
bind_class(pybind11::module& module, std::false_type)
{
    return pybind11::class_<Class,
           typename decltype(Class::Meta::base())::type>(module, Class::Meta::name());
}

// not derived class
template<typename Class>
decltype(auto)
bind_class(pybind11::module& module, std::true_type)
{
    return pybind11::class_<Class>(module, Class::Meta::name());
}

template<typename Class>
void bind_with_pybind(pybind11::module& module)
{
    auto c = bind_class<Class>(module, std::is_same<decltype(Class::Meta::base()), Type<void>>{});
    c.def(pybind11::init());

    visit([&c](auto const& name_member) {
            c.def_readwrite(name_member.first, name_member.second);
            }, Class::Meta::fields());
}

