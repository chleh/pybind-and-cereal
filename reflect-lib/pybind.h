#pragma once

#include <utility>

#include <pybind11/pybind11.h>

#include "reflect-macros.h"


// #include <iostream>
// #include <typeinfo>

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
           typename Class::Meta::base>(module, Class::Meta::name());
}

// not derived class
template<typename Class>
decltype(auto)
bind_class(pybind11::module& module, std::true_type)
{
    return pybind11::class_<Class>(module, Class::Meta::name());
}

template <class Class, class... Cs>
decltype(auto)
add_ctor(pybind11::class_<Class, Cs...>& c)
{
    return add_ctor_impl(c, std::is_abstract<Class>{});
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_impl(pybind11::class_<Class, Cs...>& c, std::true_type)
{
    return c;
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_impl(pybind11::class_<Class, Cs...>& c, std::false_type)
{
    return c.def(pybind11::init());
}

template<typename T>
struct ResultType;

template<typename Res, typename Class>
struct ResultType<Res Class::*>
{
    using type = Res;
};

template<typename Class>
struct Visitor
{
    Class& c;

    template <typename T>
    void operator()(std::pair<const char*, T> const& name_member) const
    {
        op_impl(name_member, Type<typename ResultType<decltype(name_member.second)>::type>{});
    }

    template <typename T, typename Res>
    void op_impl(std::pair<const char*, T> const& name_member, Type<Res>) const
    {
        c.def_readwrite(name_member.first, name_member.second);
        // std::cout << "Res: " << typeid(Res).name() << "\n";
    }

    template <typename T, typename... Ts>
    void op_impl(std::pair<const char*, T> const& name_member, Type<std::unique_ptr<Ts...>>) const
    {
        // TODO: what to do with them?
        // std::cout << "unique_ptr: _" << name_member.first << "_\n";
    }
};

template<typename Class>
Visitor<Class> makeVisitor(Class& c) {
    return Visitor<Class>{c};
}

template<typename Class>
void bind_with_pybind(pybind11::module& module)
{
    auto c = bind_class<Class>(module, std::is_same<typename Class::Meta::base, void>{});
    add_ctor(c);

    // visit([&c](auto const& name_member) {
    //         c.def_readwrite(name_member.first, name_member.second);
    //         }, Class::Meta::fields());
    auto v = makeVisitor(c);
    visit(v, Class::Meta::fields());
    visit([&c](auto const& name_member) {
            c.def(name_member.first, name_member.second);
            }, Class::Meta::methods());
}

