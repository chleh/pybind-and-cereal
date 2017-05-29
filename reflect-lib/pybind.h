#pragma once

#include <utility>

#include <pybind11/pybind11.h>


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


template<typename Class>
void wrap_into_pybind(pybind11::module& module)
{
    auto c = pybind11::class_<Class>(module, Class::Meta::name())
        .def(pybind11::init());

    visit([&c](auto const& name_member) {
            c.def_readwrite(name_member.first, name_member.second);
            }, Class::Meta::fields());
}
