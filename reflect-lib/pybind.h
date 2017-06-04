#pragma once

#include <string>
#include <typeinfo>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include "reflect-macros.h"
#include "remangle.h"
#include "smart_ptr.h"


#include <iostream>



PYBIND11_DECLARE_HOLDER_TYPE(T, smart_ptr<T>);


namespace detail
{

struct NoOp
{
    template<typename... Ts>
    NoOp(Ts&&...)
    {
    }
};


template <typename Visitor, typename... Ts, std::size_t... Idcs>
void visit_impl(Visitor&& v, std::tuple<Ts...>&& t, std::index_sequence<Idcs...>)
{
    NoOp{ (v(std::forward<Ts>(std::get<Idcs>(t))), 0) ... };
}

template <typename Visitor, typename... Ts>
void visit(Visitor&& v, std::tuple<Ts...>&& t)
{
    using Idcs = std::index_sequence_for<Ts...>;
    visit_impl(std::forward<Visitor>(v), std::forward<std::tuple<Ts...>>(t), Idcs{});
}

// derived class
template<typename Class>
decltype(auto)
bind_class(pybind11::module& module, std::false_type)
{
    return pybind11::class_<Class,
           typename Class::Meta::base,
           smart_ptr<Class>
               >(module, remangle(Class::Meta::mangled_name()).c_str());
}

// not derived class
template<typename Class>
decltype(auto)
bind_class(pybind11::module& module, std::true_type)
{
    return pybind11::class_<Class,
           smart_ptr<Class>>(module, remangle(Class::Meta::mangled_name()).c_str());
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

template <class Class, class... Cs>
decltype(auto)
add_ctor(pybind11::class_<Class, Cs...>& c)
{
    // TODO add copy ctor
    // move ctor not supported
    // TODO add aggregate ctor if type is aggegate (issue warning if wrongly
    // inferred) --> CPPCON talk by Yandex guy
    return add_ctor_impl(c, std::is_abstract<Class>{});
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

template<typename T>
struct TypeFeatures
{
    // static constexpr bool is_default_constructible
    //     = std::is_default_constructible<T>::value;

    static constexpr bool is_copy_assignable
        = std::is_copy_assignable<T>::value;

    static constexpr bool is_move_assignable
        = std::is_move_assignable<T>::value;

    // TODO is_const (Maybe not needed because not supported by python

    using features = std::integer_sequence<bool,
          // is_default_constructible,
          is_copy_assignable, is_move_assignable>;
};

template<typename PybindClass>
struct Visitor
{
    PybindClass& c;
    pybind11::module& m;
    pybind11::dict& all_types;

    template <typename MemberPtr>
    void operator()(std::pair<const char*, MemberPtr> const& name_member) const
    {
        using Res = typename ResultType<MemberPtr>::type;
        op_impl(name_member, Type<Res>{});
    }

private:
    template <typename MemberPtr, typename Res>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res> t) const
    {
        op_impl(name_member, t, typename TypeFeatures<Res>::features{});
    }

    template <typename MemberPtr, typename UniqueT, typename UniqueD, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<std::unique_ptr<UniqueT, UniqueD>>) const
    {
        // TODO support assigning nullptr
        auto const& member_pointer = name_member.second;
        using Class = typename GetClass<MemberPtr>::type;
        // using Res   = typename ResultType<MemberPtr>::type;
        pybind11::cpp_function fget(
                [member_pointer](Class& c) -> UniqueT* {
                    return (c.*member_pointer).get();
                },
                pybind11::is_method(this->c));
        c.def_property_readonly(name_member.first,
                fget, pybind11::return_value_policy::reference_internal);

        std::string const name = name_member.first;
        // TODO reenable if also examining if UniqueT is final
        // if (std::is_copy_constructible<UniqueT>::value)
        {
            auto const py_name = name + "__COPY_IN";
            pybind11::cpp_function fget(
                    [name, py_name](Class& c) -> UniqueT* {
                        throw pybind11::key_error{"The member \"" + py_name + "\" is not intended for read access of "
                            "the data member \"" + name + "\".\n"
                            "Rather, \"" + py_name + "\" shall only be used for copy-constructing "
                            "the C++ object held by the member \"" + name + "\" (\"" + name + "\" being a std::unique_ptr), i.e., in "
                            "Python code like:\n"
                            "    obj." + py_name + " = rhs\n"
                            "For read access to \"" + name + "\" please use the member \"" + name + "\" instead."};
                    },
                    pybind11::is_method(this->c));
            pybind11::cpp_function fset(
                    [member_pointer](Class& c, smart_ptr<UniqueT>& value) {
                        (c.*member_pointer).reset(value.new_copied());
                    },
                    pybind11::is_method(this->c));

            c.def_property(py_name.c_str(), fget, fset);
        }
        // if (std::is_move_constructible<UniqueT>::value)
        {
            auto const py_name = name + "__MOVE_IN";
            pybind11::cpp_function fget(
                    [name, py_name](Class& c) -> UniqueT* {
                        throw pybind11::key_error{"The member \"" + py_name + "\" is not intended for read access of "
                            "the data member \"" + name + "\".\n"
                            "Rather, \"" + py_name + "\" shall only be used for move-constructing "
                            "the C++ object held by the member \"" + name + "\" (\"" + name + "\" being a std::unique_ptr), i.e., in "
                            "Python code like:\n"
                            "    obj." + py_name + " = rhs\n"
                            "For read access to \"" + name + "\" please use the member \"" + name + "\" instead."};
                    },
                    pybind11::is_method(this->c));
            pybind11::cpp_function fset(
                    [member_pointer](Class& c, smart_ptr<UniqueT>& value) {
                        (c.*member_pointer).reset(value.new_moved());
                    },
                    pybind11::is_method(this->c));

            c.def_property(py_name.c_str(), fget, fset);
        }
    }

    // TODO: std::map, std::set, std::list, std::...
    template <typename MemberPtr, typename VecElem, typename VecAlloc, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<std::vector<VecElem, VecAlloc>>) const
    {
        auto const vec_type_name = demangle(typeid(std::vector<VecElem, VecAlloc>).name());
        // Check if the auxiliary std::vector binding already exists.
        if (!all_types.contains(vec_type_name.c_str()))
        {
            // TODO this procedure might create lots of duplicate binding code in many
            // different modules
            auto vec_c = pybind11::bind_vector<std::vector<VecElem, VecAlloc>>(
                    m, mangle(vec_type_name), pybind11::buffer_protocol());
            all_types[vec_type_name.c_str()] = vec_c;
        }
        c.def_readwrite(name_member.first, name_member.second);
    }


    // return reference by default --> is already pybind default for getters (according to docs)
    template <typename MemberPtr, typename Res, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res>, std::integer_sequence<bool, true /*copy*/, Feats...>
            ) const
    {
        // TODO use pattern like move-only and add specialization for shared_ptr
        // TODO what about copyable and movable types?
        c.def_readwrite(name_member.first, name_member.second);
    }

    template <typename MemberPtr, typename Res, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res>, std::integer_sequence<bool, false /*copy*/, true /*move*/, Feats...>
            ) const
    {
        c.def_readonly(name_member.first, name_member.second);

        using Class = typename GetClass<MemberPtr>::type;
        auto const& member_pointer = name_member.second;
        std::string const name = name_member.first;
        auto const py_name = name + "__MOVE_IN";
        pybind11::cpp_function fget(
                [name, py_name](Class& c) -> Res* {
                    throw pybind11::key_error{"The member \"" + py_name + "\" is not intended for read access of "
                        "the data member \"" + name + "\".\n"
                        "Rather, \"" + py_name + "\" shall only be used for move-assigning the C++ data, i.e., in "
                        "Python code like:\n"
                        "    obj." + py_name + " = rhs\n"
                        "For read access to \"" + name + "\" please use the member \"" + name + "\" instead."};
                },
                pybind11::is_method(this->c));
        pybind11::cpp_function fset(
                [member_pointer](Class& c, Res& value) {
                    c.*member_pointer = std::move(value);
                },
                pybind11::is_method(this->c));
        c.def_property(py_name.c_str(), fget, fset);
    }

    // TODO:
    //  * pointer members (Maybe not, because conflicts with cereal)
    //  * refactor to add_get, add_copy_assign, add_move_assign
};

template<typename Class, typename... Args>
Visitor<Class> makeVisitor(Class& c, Args&&... args) {
    return Visitor<Class>{c, std::forward<Args>(args)...};
}

}  // namespace detail


template<typename Class>
void bind_with_pybind(pybind11::module& module)
{
    if (!pybind11::hasattr(module, "all_types")) {
        module.add_object("all_types", pybind11::dict{});
    }
    auto all_types = pybind11::getattr(module, "all_types").cast<pybind11::dict>();

    // create class
    auto c = detail::bind_class<Class>(module, std::is_same<typename Class::Meta::base, void>{});

    // register in type list
    auto const name = demangle(Class::Meta::mangled_name());
    all_types[name.c_str()] = c;

    // add constructor
    detail::add_ctor(c);

    // add data fields
    auto v = detail::makeVisitor(c, module, all_types);
    detail::visit(v, Class::Meta::fields());

    // add methods
    detail::visit([&c](auto const& name_member) {
            c.def(name_member.first, name_member.second);
            }, Class::Meta::methods());
}

