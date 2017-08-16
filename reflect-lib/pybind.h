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
#include "util.h"

#include <iostream>

// Why not unique_ptr? --> Because it has to be copyable.
PYBIND11_DECLARE_HOLDER_TYPE(T, reflect_lib::smart_ptr<T>)

namespace reflect_lib
{
struct Module {
    explicit Module(pybind11::module module_) : module(module_)
    {
        if (!pybind11::hasattr(module, "all_types")) {
            module.add_object("all_types", pybind11::dict{});
        }
        all_types =
            pybind11::getattr(module, "all_types").cast<pybind11::dict>();
    }

    template <typename Class, typename... Options>
    pybind11::class_<Class, Options...> bind();

    pybind11::module module;
    pybind11::dict all_types;
    std::unique_ptr<std::string> namespace_name;
    pybind11::module aux_module;
    pybind11::dict aux_types;
};


namespace detail
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
    std::unique_ptr<T> const& getConst() const { return p_; }
    std::unique_ptr<T> && getRValue() { return std::move(p_); }

private:
    UniquePtrReference(T* p) : p_(p) {}
    std::unique_ptr<T> p_;
};

template <typename T>
UniquePtrReference<T>
copy_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    return UniquePtrReference<T>::new_copied(p);
}

template <typename T>
UniquePtrReference<T>
move_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    return UniquePtrReference<T>::new_moved(p);
}

template <typename T>
class RValueReference
{
public:
    static RValueReference<T> new_copied(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_copied()};
    }
    static RValueReference<T> new_moved(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_moved()};
    }

    T&& get() { return std::move(*p_); }

private:
    RValueReference(T* p) : p_(p) {}
    std::unique_ptr<T> p_;
};

template <typename T>
RValueReference<T>
copy_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_copied(p);
}

template <typename T>
RValueReference<T>
move_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_moved(p);
}


// derived class
template <typename Class, typename... Options>
decltype(auto) bind_class(pybind11::module& module, std::false_type)
{
    static_assert(std::is_base_of<typename Class::Meta::base, Class>::value,
                  "The current class is not derived from the specified base.");
    return pybind11::class_<Class, typename Class::Meta::base,
                            smart_ptr<Class>, Options...>(
        module,
        mangle(strip_namespaces(demangle(Class::Meta::mangled_name())))
            .c_str());
}

// not derived class
template <typename Class, typename... Options>
decltype(auto) bind_class(pybind11::module& module, std::true_type)
{
    return pybind11::class_<Class, smart_ptr<Class>, Options...>(
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
    /* check for trampoline class */
    constexpr bool has_trampoline = pybind11::class_<Class, Cs...>::has_alias;
    using trampoline = typename pybind11::class_<Class, Cs...>::type_alias;
    constexpr bool t_has_def_ctor =
        has_trampoline && std::is_default_constructible<trampoline>::value;

    add_ctor_if_default_constructible(
        c, std::integral_constant < bool,
        std::is_default_constructible<Class>::value || t_has_def_ctor > {});

    // TODO extend for trampoline classes?
    return add_ctor_if_copy_constructible(c,
                                          std::is_copy_constructible<Class>{});
}

template <typename... Ts>
struct GetFieldTypes
{
private:
    using IndexSequence = std::index_sequence_for<Ts...>;

    template <typename Tuple, std::size_t... Indices>
    static decltype(auto) get_types(Tuple, std::index_sequence<Indices...>)
        -> std::tuple<typename ResultType<decltype(
            std::get<Indices>(std::declval<Tuple>()).second)>::type...>;

public:
    using type =
        decltype(get_types(std::declval<std::tuple<Ts...>>(), IndexSequence{}));
    // using type = std::tuple<decltype(std::declval<Ts>()))...>
};

template <class Class, class... Options, typename... Ts>
decltype(auto) add_ctor_impl(pybind11::class_<Class, Options...>&c ,
                             std::tuple<Ts...>*, std::false_type)
{
    // std::cout << "no special ctors\n";
    return c;
}

template <class Class, class... Options, typename... Ts>
decltype(auto) add_ctor_impl(pybind11::class_<Class, Options...>& c,
                             std::tuple<Ts...>*, std::true_type)
{
    // std::cout << "some special ctors\n";
    return c.def(pybind11::init<Ts...>());;
}

template <class Class, class... Options, typename... Ts>
decltype(auto)
add_ctor_impl(pybind11::class_<Class, Options...>& c, std::tuple<Ts...>* t)
{
#if 0
    std::cout << "ZZ " << demangle(typeid(Class).name()) << '\n';
    std::cout << "zz "
              << demangle(
                     typeid(std::tuple<typename std::remove_const<Ts>::type...>)
                         .name())
              << '\n';
#endif
    return add_ctor_impl(
        c, t, std::is_constructible<Class,
                                    typename std::remove_const<Ts>::type...>{});
}

// TODO trampoline classes
template <class Class, class... Options, typename... Ts>
decltype(auto)
add_ctor(pybind11::class_<Class, Options...>& c, std::tuple<Ts...>)
{
    using FieldTypes = typename GetFieldTypes<Ts...>::type;
    std::cout << "XX " << demangle(typeid(FieldTypes).name()) << '\n';

    return add_ctor_impl(c, static_cast<FieldTypes*>(nullptr));
}

template<typename T>
struct GetClass;

template<typename Res, typename Class>
struct GetClass<Res Class::*>
{
    using type = Class;
};

template <typename T>
struct GetArgumentType;

template <typename R, typename C, typename A>
struct GetArgumentType<R C::*(A)> {
    using type = A;
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

struct AddAuxTypeGeneric {
    template <typename Fct>
    static void add_type_checked(Fct&& f, std::string name, Module& m)
    {
        auto const mangled_name = mangle(name);

        if (!pybind11::hasattr(m.aux_module, mangled_name.c_str())) {
            auto const aux = f(mangled_name, m);

            if (static_cast<bool>(aux)) {
                if (m.all_types.contains(name.c_str()))
                    throw std::logic_error(
                        "Binding present in all_types, but not yet in aux "
                        "module.");
                if (m.aux_types.contains(name.c_str()))
                    throw std::logic_error(
                        "Binding present in aux_types, but not yet in aux "
                        "module.");

                m.all_types[name.c_str()] = aux;
                m.aux_types[name.c_str()] = aux;
            }
        }
    }
};

template <typename T>
struct AddAuxType {
    static void add(Module& /*m*/) {}
};

template <typename VecElem, typename VecAlloc>
struct AddAuxType<std::vector<VecElem, VecAlloc>>
{
private:
    using Vec = std::vector<VecElem, VecAlloc>;

public:
    static void add(Module& m)
    {
        auto const type_name =
            demangle(typeid(Vec).name());

        AddAuxTypeGeneric::add_type_checked([](
            std::string const& mangled_type_name,
            Module& m) {
            auto vec_c = pybind11::bind_vector<Vec, smart_ptr<Vec>>(
                m.aux_module, mangled_type_name, pybind11::buffer_protocol());
            return vec_c;
        }, type_name, m);
    }
};

template <typename T>
struct AddAuxType<UniquePtrReference<T>>
{
private:
    using Ref = UniquePtrReference<T>;

public:
    static void add(Module& m)
    {
        auto const type_name = demangle(typeid(Ref).name());

        AddAuxTypeGeneric::add_type_checked(
            [](std::string const& mangled_type_name, Module& m) {
                auto ref_c = pybind11::class_<Ref, smart_ptr<Ref>>(
                    m.aux_module, mangled_type_name.c_str());
                return ref_c;
            },
            type_name, m);

        // TODO avoid duplicate bindings
        m.aux_module.def("copy_to_unique_ptr", &copy_to_unique_ptr<T>);
        m.aux_module.def("move_to_unique_ptr", &move_to_unique_ptr<T>);
    }
};

template <typename T>
struct AddAuxType<RValueReference<T>>
{
private:
    using Ref = RValueReference<T>;

public:
    static void add(Module& m)
    {
        auto const type_name = demangle(typeid(Ref).name());

        AddAuxTypeGeneric::add_type_checked(
            [](std::string const& mangled_type_name, Module& m) {
                auto ref_c = pybind11::class_<Ref, smart_ptr<Ref>>(
                    m.aux_module, mangled_type_name.c_str());
                return ref_c;
            },
            type_name, m);

        // TODO avoid duplicate bindings
        m.aux_module.def("copy_to_rvalue_reference",
                         &copy_to_rvalue_reference<T>);
        m.aux_module.def("move_to_rvalue_reference",
                         &move_to_rvalue_reference<T>);
    }
};

template <typename T>
void add_aux_type(Type<T> t, Module& m)
{
    // TODO this procedure might create lots of duplicate binding code in many
    // different modules
    // std::cout << "binding aux " << typeid(T).name() << '\n';

    if (!m.aux_module) {
        // TODO make auxiliary module name configurable?
        m.aux_module =
            pybind11::module::import("auxiliary_types_autogenerated");

        if (!pybind11::hasattr(m.aux_module, "aux_types")) {
            m.aux_module.add_object("aux_types", pybind11::dict{});
        }
        m.aux_types = pybind11::getattr(m.aux_module, "aux_types")
                          .template cast<pybind11::dict>();
    }

    AddAuxType<T>::add(m);
}

template<typename PybindClass>
struct DefFieldsVisitor
{
    PybindClass& c;
    reflect_lib::Module& module;

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

    template <typename MemberPtr, typename Res>
    void op_impl(std::pair<const char*, MemberPtr> const& name_member,
            Type<Res const> t) const
    {
        c.def_readonly(name_member.first, name_member.second);
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
        add_aux_type(t, module);
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


// argument converters /////////////////////////////////////////////////////////

template <typename CPPType>
struct ArgumentConverter {
    using PyType = CPPType;

    static decltype(auto) convert(PyType&& o)
    {
        return std::forward<PyType>(o);
    }
};

template <typename CPPType>
struct ArgumentConverter<CPPType&&> {
    static CPPType&& convert(RValueReference<CPPType>& o)
    {
        return o.get();
    }

    // TODO static_assert that this is reference type? (for all
    // ArgumentConverter types)
    using PyType = RValueReference<CPPType>&;
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>> {
    static std::unique_ptr<P, D>&& convert(UniquePtrReference<P>& p)
    {
        return p.getRValue();
    }

    using PyType = UniquePtrReference<P>&;
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>&&> {
    static std::unique_ptr<P, D>&& convert(UniquePtrReference<P>& p)
    {
        return p.getRValue();
    }

    using PyType = UniquePtrReference<P>&;
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>&> {
    static std::unique_ptr<P, D>& convert(UniquePtrReference<P>& p)
    {
        return p.get();
    }

    using PyType = UniquePtrReference<P>&;
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D> const&> {
    static std::unique_ptr<P, D> const& convert(UniquePtrReference<P> const& p)
    {
        return p.getConst();
    }

    using PyType = UniquePtrReference<P> const&;
};

// end argument converters /////////////////////////////////////////////////////


// return value converters /////////////////////////////////////////////////////

template <typename CPPType>
struct ReturnValueConverter {
    static decltype(auto) convert(CPPType&& o)
    {
        return std::forward<CPPType>(o);
    }

    using PyType = CPPType;
};

template <>
struct ReturnValueConverter<void> {
    using PyType = void;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>> {
    static smart_ptr<P> convert(std::unique_ptr<P, D> p)
    {
        // TODO: might create wrong copy/move function!
        return smart_ptr<P>(p.release());
    }

    using PyType = typename ResultType<decltype(convert)>::type;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>&&> {
    static std::unique_ptr<P, D> const& convert(std::unique_ptr<P, D>&& p)
    {
        // TODO: might create wrong copy/move function!
        return smart_ptr<P>(p.release());
    }

    using PyType = typename ResultType<decltype(convert)>::type;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>&> {
    static UniquePtrReference<P> convert(std::unique_ptr<P, D>& p)
    {
        return UniquePtrReference<P>(p.get());
    }

    using PyType = typename ResultType<decltype(convert)>::type;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D> const&> {
    static UniquePtrReference<P> convert(std::unique_ptr<P, D> const& p)
    {
        // TODO maybe just return p.get() ?
        return UniquePtrReference<P>(p.get());
    }

    using PyType = typename ResultType<decltype(convert)>::type;
};

// end return value converters /////////////////////////////////////////////////


template<typename PybindClass>
struct DefMethodsVisitor
{
    PybindClass& c;
    Module& module;

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

    template <typename MemberFctPtr, typename Res, typename Class,
              typename... Args>
    decltype(auto) static wrap_method(MemberFctPtr member_ptr,
                                      Res (Class::*)(Args...))
    {
        return [member_ptr](Class & c,
                            typename ArgumentConverter<Args>::PyType... args) ->
               typename ReturnValueConverter<Res>::PyType
        {
            return ReturnValueConverter<Res>::convert(
                (c.*member_ptr)(ArgumentConverter<Args>::convert(
                    std::forward<typename ArgumentConverter<Args>::PyType>(
                        args))...));
        };
    }

    template <typename MemberFctPtr, typename Class, typename... Args>
    decltype(auto) static wrap_method(MemberFctPtr member_ptr,
                                      void (Class::*)(Args...))
    {
        return [member_ptr](Class& c,
                            typename ArgumentConverter<Args>::PyType... args) {
            (c.*member_ptr)(ArgumentConverter<Args>::convert(
                std::forward<typename ArgumentConverter<Args>::PyType>(
                    args))...);
        };
    }

    template <typename MemberFctPtr, typename Res, typename Class,
              typename... Args>
    void op_impl(const char* name, MemberFctPtr member_ptr,
                 Res (Class::*p)(Args...)) const
    {
        static_assert(
            !std::is_pointer<Res>::value,
            "Methods returning pointers are not supported by this library.");

        pybind11::return_value_policy policy =
            pybind11::return_value_policy::automatic;

        if (std::is_lvalue_reference<Res>::value) {
            policy = pybind11::return_value_policy::reference_internal;
        }

        auto wrapped_method = wrap_method(member_ptr, p);

        // add auxiliary bindings
        visit([&](auto t) { add_aux_type(t, module); },
              std::make_tuple(
                  Type<typename std::decay<
                      typename ReturnValueConverter<Res>::PyType>::type>{},
                  Type<typename std::decay<
                      typename ArgumentConverter<Args>::PyType>::type>{}...));
        c.def(name, wrapped_method, policy);
    }
};

template<typename Class, typename... Args>
DefMethodsVisitor<Class> makeDefMethodsVisitor(Class& c, Args&&... args) {
    return DefMethodsVisitor<Class>{c, std::forward<Args>(args)...};
}

}  // namespace detail


template <typename Class, typename... Options>
pybind11::class_<Class, Options...> Module::bind()
{
    // create class
    auto c = detail::bind_class<Class, Options...>(
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
    detail::add_ctor(c, Class::Meta::fields());

    // add data fields
    visit(detail::makeDefFieldsVisitor(c, *this),
                  Class::Meta::fields());

    // add methods
    visit(detail::makeDefMethodsVisitor(c, *this),
                  Class::Meta::methods());

    return c;
}

#define CONCAT(a, b, c) CONCAT_(a, b, c)
#define CONCAT_(a, b, c) a ## b ## c

#define REFLECT_LIB_PYTHON_MODULE(name, variable) \
    APPLY(PYBIND11_MODULE,                        \
          CONCAT(REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX, __, name), variable)

}  // namespace reflect_lib
