#pragma once

#include <cassert>
#include <string>
#include <typeinfo>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include "reflect-macros.h"
#include "remangle.h"
#include "smart_ptr.h"

#include <iostream>

// Why not unique_ptr? --> Because it has to be copyable.
PYBIND11_DECLARE_HOLDER_TYPE(T, reflect_lib::smart_ptr<T>);

namespace reflect_lib
{

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
template <typename Class>
decltype(auto) bind_class(pybind11::module& module, std::false_type)
{
    static_assert(std::is_base_of<typename Class::Meta::base, Class>::value,
                  "The current class is not derived from the specified base.");
    return pybind11::class_<Class, typename Class::Meta::base,
                            smart_ptr<Class>>(
        module,
        mangle(strip_namespaces(demangle(Class::Meta::mangled_name())))
            .c_str());
}

// not derived class
template <typename Class>
decltype(auto) bind_class(pybind11::module& module, std::true_type)
{
    return pybind11::class_<Class, smart_ptr<Class>>(
        module,
        mangle(strip_namespaces(demangle(Class::Meta::mangled_name())))
            .c_str());
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_if_default_constructible(pybind11::class_<Class, Cs...>& c, std::true_type)
{
    return c.def(pybind11::init());
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_if_default_constructible(pybind11::class_<Class, Cs...>& c, std::false_type)
{
    return c;
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_if_copy_constructible(pybind11::class_<Class, Cs...>& c, std::true_type)
{
    return c.def(pybind11::init<Class const&>());
}

template <class Class, class... Cs>
decltype(auto)
add_ctor_if_copy_constructible(pybind11::class_<Class, Cs...>& c, std::false_type)
{
    return c;
}

template <class Class, class... Cs>
decltype(auto)
add_ctor(pybind11::class_<Class, Cs...>& c)
{
    add_ctor_if_default_constructible(c, std::is_default_constructible<Class>{});
    return add_ctor_if_copy_constructible(c, std::is_copy_constructible<Class>{});
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
    static constexpr bool is_copy_assignable
        = std::is_copy_assignable<T>::value;

    static constexpr bool is_move_assignable
        = std::is_move_assignable<T>::value;

    using features = std::integer_sequence<bool,
          is_copy_assignable, is_move_assignable>;
};

template <typename T>
void add_aux_type_impl(T, pybind11::module&, pybind11::dict&)
{
}

// TODO: std::map, std::set, std::list, std::...
template <typename VecElem, typename VecAlloc>
void add_aux_type_impl(Type<std::vector<VecElem, VecAlloc>>,
        pybind11::module& m,
        pybind11::dict& all_types)
{
    using Vec = std::vector<VecElem, VecAlloc>;
    auto const vec_type_name = demangle(typeid(Vec).name());
    // Check if the auxiliary std::vector binding already exists.
    if (!all_types.contains(vec_type_name.c_str()))
    {
        // TODO this procedure might create lots of duplicate binding code in many
        // different modules
        auto vec_c = pybind11::bind_vector<Vec, smart_ptr<Vec>>(
                m, mangle(vec_type_name), pybind11::buffer_protocol());
        all_types[vec_type_name.c_str()] = vec_c;
    }
}

template <typename T>
void add_aux_type(T t, pybind11::module& m, pybind11::dict& all_types)
{
    // TODO proper fix for this doubly-registered type error workaround
    try {
        add_aux_type_impl(t, m, all_types);
    } catch (std::runtime_error /*e*/) {
        // std::cerr << "ERROR: " << e.what() << '\n';
    }
}

template<typename PybindClass>
struct DefFieldsVisitor
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

    // member of type std::unique_ptr
    template <typename MemberPtr, typename UniqueT, typename UniqueD, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<std::unique_ptr<UniqueT, UniqueD>>) const
    {
        auto const& member_pointer = name_member.second;
        using Class = typename GetClass<MemberPtr>::type;
        pybind11::cpp_function fget(
                [member_pointer](Class& c) -> UniqueT* {
                    return (c.*member_pointer).get();
                },
                pybind11::is_method(this->c),
                pybind11::return_value_policy::reference_internal
                );
        // reset with nullptr
        auto fset = [member_pointer](Class& c, std::nullptr_t) {
            (c.*member_pointer).reset();
        };
        c.def_property(name_member.first, fget, fset);

        std::string const name = name_member.first;
        // TODO reenable if also examining if UniqueT is final
        // if (std::is_copy_constructible<UniqueT>::value)
        {
            // copy-construct held object
            // TODO maybe copy-assign? --> Con: needs to check that !nullptr
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
            // move-construct held object
            // TODO maybe move-assign? --> Con: needs to check that !nullptr
            auto const py_name = name + "__MOVE_IN";
            pybind11::cpp_function fget(
                    [name, py_name](Class& c) -> UniqueT* {
                        throw pybind11::key_error("The member \"" + py_name + "\" is not intended for read access of "
                            "the data member \"" + name + "\".\n"
                            "Rather, \"" + py_name + "\" shall only be used for move-constructing "
                            "the C++ object held by the member \"" + name + "\" (\"" + name + "\" being a std::unique_ptr), i.e., in "
                            "Python code like:\n"
                            "    obj." + py_name + " = rhs\n"
                            "For read access to \"" + name + "\" please use the member \"" + name + "\" instead.");
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
    // member of type std::vector
    template <typename MemberPtr, typename VecElem, typename VecAlloc, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<std::vector<VecElem, VecAlloc>> t) const
    {
        add_aux_type(t, m, all_types);
        c.def_readwrite(name_member.first, name_member.second);
    }

    // copyable member
    template <typename MemberPtr, typename Res, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res>, std::integer_sequence<bool, true /*copy*/, Feats...>
            ) const
    {
        // TODO use pattern like move-only and add specialization for shared_ptr
        // TODO what about copyable and movable types?
        c.def_readwrite(name_member.first, name_member.second);
    }

    // move-only member
    template <typename MemberPtr, typename Res, bool... Feats>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res>, std::integer_sequence<bool, false /*copy*/, true /*move*/, Feats...>
            ) const
    {
        // provide read access
        c.def_readonly(name_member.first, name_member.second);

        // move-construct member
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

    // TODO: refactor to add_get, add_copy_assign, add_move_assign
};

template<typename Class, typename... Args>
DefFieldsVisitor<Class> makeDefFieldsVisitor(Class& c, Args&&... args) {
    return DefFieldsVisitor<Class>{c, std::forward<Args>(args)...};
}

template<typename PybindClass>
struct DefMethodsVisitor
{
    PybindClass& c;
    pybind11::module& m;
    pybind11::dict& all_types;

    template <typename MemberFctPtr>
    void operator()(std::pair<const char*, MemberFctPtr> const& name_member) const
    {
        op_impl(name_member.first, name_member.second, name_member.second);
    }

private:
    template<typename MemberFctPtr, typename Res, typename Class, typename... Args>
    void op_impl(const char* name, MemberFctPtr member_ptr, Res (Class::*)(Args...) const) const
    {
        op_impl(name, member_ptr, static_cast<Res (Class::*)(Args...)>(nullptr));
    }

    template<typename MemberFctPtr, typename Res, typename Class, typename... Args>
    void op_impl(const char* name, MemberFctPtr member_ptr, Res (Class::*)(Args...)) const
    {
        static_assert(!std::is_pointer<Res>::value,
                "Methods returning pointers are not supported by this library.");

        pybind11::return_value_policy policy = pybind11::return_value_policy::automatic;

        if (std::is_lvalue_reference<Res>::value) {
            policy = pybind11::return_value_policy::reference_internal;
        }

        // add auxiliary bindings
        visit([&](auto t) {
                add_aux_type(t, m, all_types);
                }, std::make_tuple(Type<typename std::decay<Res>::type>{},
                    Type<typename std::decay<Args>::type>{}...));
        c.def(name, member_ptr, policy);
    }

    // method returning std::unique_ptr
    template<typename MemberFctPtr, typename UniqueT, typename UniqueD,
        typename Class, typename... Args>
    void op_impl(const char* name, MemberFctPtr member_ptr,
            std::unique_ptr<UniqueT, UniqueD> (Class::*)(Args...)) const
    {
        // add auxiliary bindings
        visit([&](auto t) {
                add_aux_type(t, m, all_types);
                }, std::make_tuple(Type<typename std::decay<UniqueT>::type>{},
                    Type<typename std::decay<Args>::type>{}...));
        auto f = [member_ptr](Class& c, Args... args) {
            return smart_ptr<UniqueT>{
                (c.*member_ptr)(std::forward<Args>(args)...).release()};
        };
        c.def(name, f);
    }
    // TODO returning unique_ptr&, unique_ptr const&
    // TODO taking unique_ptr&, unique_ptr const&, unique_ptr&&, unique_ptr as argument?
};

template<typename Class, typename... Args>
DefMethodsVisitor<Class> makeDefMethodsVisitor(Class& c, Args&&... args) {
    return DefMethodsVisitor<Class>{c, std::forward<Args>(args)...};
}

}  // namespace detail

#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a ## b

#define REFLECT_LIB_PYTHON_MODULE(name, variable)                     \
    APPLY(PYBIND11_MODULE,                                            \
          CONCAT(REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX, name), \
          variable)

struct Module {
    explicit Module(pybind11::module& module_) : module(module_)
    {
        if (!pybind11::hasattr(module, "all_types")) {
            module.add_object("all_types", pybind11::dict{});
        }
        all_types =
            pybind11::getattr(module, "all_types").cast<pybind11::dict>();
    }

    template <typename Class>
    decltype(auto) bind()
    {
        // create class
        auto c = detail::bind_class<Class>(
            module, std::is_same<typename Class::Meta::base, void>{});

        // register in type list
        auto const full_name = demangle(Class::Meta::mangled_name());
        auto const name = strip_namespaces(full_name);
        all_types[name.c_str()] = c;
        if (!namespace_name) {
            namespace_name.reset(new std::string(get_namespaces(full_name)));
        } else if (*namespace_name != get_namespaces(full_name)) {
            throw pybind11::type_error(
                "For consistence between python modules and C++ namespaces "
                "inside one module only types from one namespace are allowed. "
                "Until now types from two C++ namespaces have been introduced "
                "into this module, namely `" +
                *namespace_name + "' and `" + get_namespaces(full_name) + "'.");
        }
        // for debugging only
        // std::cout << "binding " << full_name << "\n";

        // add constructor
        detail::add_ctor(c);

        // add data fields
        detail::visit(detail::makeDefFieldsVisitor(c, module, all_types),
                      Class::Meta::fields());

        // add methods
        detail::visit(detail::makeDefMethodsVisitor(c, module, all_types),
                      Class::Meta::methods());

        return c;
    }

    pybind11::module& module;
    pybind11::dict all_types;
    std::unique_ptr<std::string> namespace_name;
};

}  // namespace reflect_lib
