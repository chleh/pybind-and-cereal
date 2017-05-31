#pragma once

#include <utility>

#include <pybind11/pybind11.h>

#include "reflect-macros.h"


#include <iostream>
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

template<typename T>
struct GetClass;

template<typename Res, typename Class>
struct GetClass<Res Class::*>
{
    using type = Class;
};

template<typename PybindClass>
struct Visitor
{
    PybindClass& c;

    template <typename MemberPtr>
    void operator()(std::pair<const char*, MemberPtr> const& name_member) const
    {
        op_impl(name_member, Type<typename ResultType<decltype(name_member.second)>::type>{});
    }

private:
    template <typename MemberPtr, typename Res>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member, Type<Res>) const
    {
        op_if_copyable(name_member, std::is_copy_constructible<Res>{});
    }

    template <typename MemberPtr, typename UniqueT, typename UniqueD>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<std::unique_ptr<UniqueT, UniqueD>>) const
    {
        auto const& member_pointer = name_member.second;
        using Class = typename GetClass<MemberPtr>::type;
        // using Res   = typename ResultType<MemberPtr>::type;
        pybind11::cpp_function fget(
                [member_pointer](Class& c) -> UniqueT* {
                    return (c.*member_pointer).get();
                },
                pybind11::is_method(this->c));
        pybind11::cpp_function fset(
                [member_pointer](Class& c, pybind11::object& value) {
                    if (value) { // TODO better check
                        auto* cpp_value = value.cast<UniqueT*>();
                        value.inc_ref(); // keeps Python from cleaning value up. probably very problematic!
                        // value.release(); // keeps Python from cleaning value up. probably very problematic!
                        (c.*member_pointer).reset(cpp_value);
                    } else {
                        (c.*member_pointer).reset();
                    }
                },
                pybind11::is_method(this->c));

        c.def_property(name_member.first,
                fget, fset,
                    pybind11::return_value_policy::reference_internal);
    }

    template <typename MemberPtr>
    void op_if_copyable(std::pair<const char*, MemberPtr> const& name_member, std::true_type) const
    {
        c.def_readwrite(name_member.first, name_member.second);
    }

    template <typename MemberPtr>
    void op_if_copyable(std::pair<const char*, MemberPtr> const& name_member, std::false_type) const
    {
        c.def_readonly(name_member.first, name_member.second);
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

