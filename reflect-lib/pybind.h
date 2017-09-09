#pragma once

#include <cassert>
#include <string>
#include <typeinfo>
#include <utility>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include "aux-types.h"
#include "reflect-macros.h"
#include "remangle.h"
#include "smart_ptr.h"
#include "stl-bind.h"
#include "type-converters.h"
#include "util.h"

// TODO check that claim!
// Why not unique_ptr? --> Because it has to be copyable.
PYBIND11_DECLARE_HOLDER_TYPE(T, reflect_lib::smart_ptr<T>)

namespace reflect_lib
{
struct Module {
    explicit Module(pybind11::module module_) : module(module_)
    {
        REFLECT_LIB_DBUG("\n========== new module ==========");
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
void add_aux_type(Type<T> t, Module& m);

// derived class
template <typename Class, typename... Options>
decltype(auto) bind_class(pybind11::module& module, std::false_type)
{
    static_assert(std::is_base_of<typename Class::Meta::base, Class>::value,
                  "The current class is not derived from the specified base.");
    return pybind11::class_<Class, typename Class::Meta::base, smart_ptr<Class>,
                            Options...>(
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
decltype(auto) add_ctor_if_default_constructible(
    pybind11::class_<Class, Cs...>& c, std::true_type)
{
    return c.def(pybind11::init(), "default constructor");
}

template <class Class, class... Cs>
decltype(auto) add_ctor_if_default_constructible(
    pybind11::class_<Class, Cs...>& c, std::false_type)
{
    return c;
}

template <class Class, class... Cs>
decltype(auto) add_ctor_if_copy_constructible(pybind11::class_<Class, Cs...>& c,
                                              std::true_type)
{
    return c.def(pybind11::init<Class const&>(), "copy constructor");
}

template <class Class, class... Cs>
decltype(auto) add_ctor_if_copy_constructible(pybind11::class_<Class, Cs...>& c,
                                              std::false_type)
{
    return c;
}

template <class Class, class... Cs>
decltype(auto) add_ctor(pybind11::class_<Class, Cs...>& c)
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
    return add_ctor_if_copy_constructible(c, IsCopyConstructible<Class>{});
}

template <typename... Ts>
struct GetFieldTypes {
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
decltype(auto) add_ctor_impl(pybind11::class_<Class, Options...>& c, Module&,
                             std::tuple<Ts...>*, std::false_type)
{
    REFLECT_LIB_DBUG("  no special ctors");
    return c;
}

template <class Class, class... Options, typename... Ts>
decltype(auto) add_ctor_impl(pybind11::class_<Class, Options...>& c,
                             Module& module, std::tuple<Ts...>*, std::true_type)
{
    REFLECT_LIB_DBUG("  some special ctors");

    // add auxiliary bindings
    visit([&](auto t) { add_aux_type(t, module); },
          std::make_tuple(
              Type<typename std::decay<
                  typename ArgumentConverter<Ts>::AuxType>::type>{}...));

    // return c.def(pybind11::init<typename
    // ArgumentConverter<Ts>::PyType...>());
    return c.def(
        "__init__",
        [](Class& instance, typename ArgumentConverter<Ts>::PyType... args) {
            new (&instance) Class(ArgumentConverter<Ts>::py2cpp(args)...);
        }, "constructor taking all of this classes' members as arguments");
}

template <class Class, class... Options, typename... Ts>
decltype(auto) add_ctor_impl(pybind11::class_<Class, Options...>& c,
                             Module& module, std::tuple<Ts...>* t)
{
#if 1
    REFLECT_LIB_DBUG("add_ctor_impl", demangle<Class>());
    REFLECT_LIB_DBUG("  ...tor_impl",
         demangle(typeid(std::tuple<typename std::remove_const<Ts>::type...>)
                      .name()));
#endif
    return add_ctor_impl(
        c, module, t,
        std::is_constructible<Class,
                              typename std::remove_const<Ts>::type...>{});
}

// TODO trampoline classes
template <class Class, class... Options, typename... Ts>
decltype(auto) add_ctor(pybind11::class_<Class, Options...>& c,
                        std::tuple<Ts...>, Module& module)
{
    using FieldTypes = typename GetFieldTypes<Ts...>::type;
    REFLECT_LIB_DBUG("add_ctor", demangle<FieldTypes>());

    return add_ctor_impl(c, module, static_cast<FieldTypes*>(nullptr));
}

template <class Class, class... Options, typename... Ts>
decltype(auto) add_pickling_impl(pybind11::class_<Class, Options...>& c,
                                 std::tuple<Ts...>*,
                                 std::false_type /* has_suitable_ctor */,
                                 std::false_type /* is_default_constructible */)
{
    return c;
}

template <typename Class, typename... PairsNameMember, typename... MemberTypes,
          std::size_t... Indices>
decltype(auto) get_state(Class const& c,
                         std::tuple<PairsNameMember...> const& fields,
                         std::tuple<MemberTypes...>*,
                         std::index_sequence<Indices...>)
{
    return pybind11::make_tuple(PickleConverter<MemberTypes>::cpp2py(
        c.*std::get<Indices>(fields).second)...);
}

template <typename Class, typename... MemberTypes, std::size_t... Indices>
void set_state(Class& c, pybind11::tuple& t, std::tuple<MemberTypes...>*,
               std::index_sequence<Indices...>)
{
    if (t.size() != sizeof...(MemberTypes))
        throw std::runtime_error("Invalid state!");

    new (&c) Class(UnpickleConverter<MemberTypes>::py2cpp(t[Indices])...);
}

template <typename Class, typename... PairsNameMember, typename... MemberTypes,
          std::size_t... Indices>
void set_state(Class& c, pybind11::tuple& t,
               std::tuple<PairsNameMember...> const& fields,
               std::tuple<MemberTypes...>*, std::index_sequence<Indices...>)
{
    static_assert(sizeof...(PairsNameMember) == sizeof...(MemberTypes),
                  "Error!");

    if (t.size() != sizeof...(PairsNameMember))
        throw std::runtime_error("Invalid state!");

    new (&c) Class();

    NoOp{(c.*std::get<Indices>(fields).second =
              UnpickleConverter<MemberTypes>::py2cpp(t[Indices]))...};
}

template <class Class, typename BoolConst, class... Options,
          typename... MemberTypes>
decltype(auto) add_pickling_impl(pybind11::class_<Class, Options...>& c,
                                 std::tuple<MemberTypes...>*,
                                 std::true_type /* has_suitable_ctor */,
                                 BoolConst /* is_default_constructible */)
{
    REFLECT_LIB_DBUG("  adding get/set state");
    using Indices = std::index_sequence_for<MemberTypes...>;
    c.def("__getstate__", [](Class const& instance) {
        return get_state(instance, Class::Meta::fields(),
                         static_cast<std::tuple<MemberTypes...>*>(nullptr),
                         Indices{});
    });
    return c.def("__setstate__", [](Class& instance, pybind11::tuple& t) {
        set_state(instance, t,
                  static_cast<std::tuple<MemberTypes...>*>(nullptr), Indices{});
    });
}

template <class Class, class... Options, typename... MemberTypes>
decltype(auto) add_pickling_impl(pybind11::class_<Class, Options...>& c,
                                 std::tuple<MemberTypes...>*,
                                 std::false_type /* has_suitable_ctor */,
                                 std::true_type /* is_default_constructible */)
{
    REFLECT_LIB_DBUG("  adding get/set state");
    using Indices = std::index_sequence_for<MemberTypes...>;
    c.def("__getstate__", [](Class const& instance) {
        return get_state(instance, Class::Meta::fields(),
                         static_cast<std::tuple<MemberTypes...>*>(nullptr),
                         Indices{});
    });
    return c.def("__setstate__", [](Class& instance, pybind11::tuple& t) {
        REFLECT_LIB_DBUG("Setting state!!!");
        set_state(instance, t, Class::Meta::fields(),
                  static_cast<std::tuple<MemberTypes...>*>(nullptr), Indices{});
    });
}

template <class Class, class... Options, typename... MemberTypes>
decltype(auto) add_pickling_impl(pybind11::class_<Class, Options...>& c,
                                 std::tuple<MemberTypes...>* t)
{
    return add_pickling_impl(
        c, t,
        std::is_constructible<Class, std::remove_const_t<MemberTypes>...>{},
        std::is_default_constructible<Class>{});
}

template <class Class, class... Options, typename... PairsNameMember>
decltype(auto) add_pickling(pybind11::class_<Class, Options...>& c,
                            std::tuple<PairsNameMember...> const&)
{
    using FieldTypes = typename GetFieldTypes<PairsNameMember...>::type;
    return add_pickling_impl(c, static_cast<FieldTypes*>(nullptr));
}

template <typename T>
struct GetClass;

template <typename Res, typename Class>
struct GetClass<Res Class::*> {
    using type = Class;
};

template <typename T>
struct GetArgumentType;

template <typename R, typename C, typename A>
struct GetArgumentType<R C::*(A)> {
    using type = A;
};

template <typename T>
struct TypeFeatures {
    static constexpr bool is_copy_assignable =
        std::is_copy_assignable<T>::value;

    static constexpr bool is_move_assignable =
        std::is_move_assignable<T>::value;

    using features =
        std::integer_sequence<bool, is_copy_assignable, is_move_assignable>;
};

struct AddAuxTypeGeneric {
    template <typename Fct>
    static bool add_type_checked(Fct&& f, std::string name, Module& m)
    {
        auto const mangled_name = mangle(name);

        if (!pybind11::hasattr(m.module, mangled_name.c_str())) {
            auto const aux = f(mangled_name, m);

            if (static_cast<bool>(aux)) {
                if (m.all_types.contains(name.c_str()))
                    throw std::logic_error(
                        "Binding present in all_types, but not yet in aux "
                        "module.");

                m.all_types[name.c_str()] = aux;
                m.aux_types[name.c_str()] = aux;

                return true;
            }
        }

        return false;
    }
};

template <typename T>
struct AddAuxType {
    static void add(Module& /*m*/) {}
};

template <typename VecElem, typename VecAlloc>
struct AddAuxType<std::vector<VecElem, VecAlloc>> {
private:
    using Vec = std::vector<VecElem, VecAlloc>;

public:
    // base case
    template <typename SFINAE = void*>
    static void add(Module& m, SFINAE = nullptr)
    {
        auto const type_name = demangle<Vec>();

        AddAuxTypeGeneric::add_type_checked(
            [](std::string const& mangled_type_name, Module& m) {
                REFLECT_LIB_DBUG("binding aux ", demangle2(mangled_type_name));
                auto vec_c = BindVector<Vec, smart_ptr<Vec>>::bind(
                    m.module, mangled_type_name);
                return vec_c;
            },
            type_name, m);
    }

    // overload for arithmetic types
    static void add(
        Module& m,
        std::enable_if_t<std::is_arithmetic<VecElem>::value>* = nullptr)
    {
        auto const type_name = demangle<Vec>();

        AddAuxTypeGeneric::add_type_checked(
            [](std::string const& mangled_type_name, Module& m) {
                REFLECT_LIB_DBUG("binding aux arith ", demangle2(mangled_type_name));
                auto vec_c = BindVector<Vec, smart_ptr<Vec>>::bind(
                    m.module, mangled_type_name);
                return vec_c;
            },
            type_name, m);
    }
};

template <typename T>
struct AddAuxType<UniquePtrReference<T>> {
private:
    using Ref = UniquePtrReference<T>;

public:
    static void add(Module& m)
    {
        auto const type_name = demangle<Ref>();
        REFLECT_LIB_DBUG("binding aux", type_name);

        if (AddAuxTypeGeneric::add_type_checked(
                [](std::string const& mangled_type_name, Module& m) {
                    auto ref_c = pybind11::class_<Ref, smart_ptr<Ref>>(
                        m.module, mangled_type_name.c_str());
                    return ref_c;
                },
                type_name, m)) {
            m.aux_module.def("copy_to_unique_ptr", &copy_to_unique_ptr<T>,
                             pybind11::arg().none(false));
            m.aux_module.def("move_to_unique_ptr", &move_to_unique_ptr<T>,
                             pybind11::arg().none(false));

            // TODO duplicate bindings problem?
            m.aux_module.def("copy_to_unique_ptr",
                             [](std::nullptr_t) { return nullptr; });
            m.aux_module.def("move_to_unique_ptr",
                             [](std::nullptr_t) { return nullptr; });
        }
    }
};

template <typename T>
struct AddAuxType<RValueReference<T>> {
private:
    using Ref = RValueReference<T>;

public:
    static void add(Module& m)
    {
        auto const type_name = demangle<Ref>();
        REFLECT_LIB_DBUG("binding aux", type_name);

        if (AddAuxTypeGeneric::add_type_checked(
                [](std::string const& mangled_type_name, Module& m) {
                    auto ref_c = pybind11::class_<Ref, smart_ptr<Ref>>(
                        m.module, mangled_type_name.c_str());
                    return ref_c;
                },
                type_name, m)) {
            m.aux_module.def("copy_to_rvalue_reference",
                             &copy_to_rvalue_reference<T>,
                             pybind11::arg().none(false));
            m.aux_module.def("move_to_rvalue_reference",
                             &move_to_rvalue_reference<T>,
                             pybind11::arg().none(false));
        }
    }
};

template <typename T>
void add_aux_type(Type<T> /*t*/, Module& m)
{
    // TODO this procedure might create lots of duplicate binding code in many
    // different modules

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

template <typename PybindClass>
struct DefFieldsVisitor {
    PybindClass& c;
    reflect_lib::Module& module;

    template <typename MemberPtr>
    void operator()(std::pair<const char*, MemberPtr> const& name_member) const
    {
        op_impl(name_member.first, name_member.second);
    }

private:
    template <typename Res, typename Class>
    void op_impl(const char* name, Res(Class::*member_ptr)) const
    {
        static_assert(!std::is_pointer<Res>::value,
                      "Pointer members are not supported by this library.");

        op_impl(name, member_ptr, std::integral_constant < bool,
                (!std::is_copy_assignable<Res>::value &&
                 !std::is_assignable<
                     Res, typename ArgumentConverter<Res>::CPPType>::value) ||
                    std::is_const<std::remove_reference_t<Res>>::value > {});
    }

    template <typename Res, typename Class>
    void op_impl(const char* name, Res(Class::*member_ptr),
                 std::true_type /* is_const */) const
    {
        REFLECT_LIB_DBUG("  readonly property: ", name, "of type",
                         demangle<Res>());
        auto getter = pybind11::cpp_function(
            [member_ptr](Class & c) ->
            typename ReturnValueConverter<Res&>::PyType {
                return ReturnValueConverter<Res&>::cpp2py(c.*member_ptr);
            },
            pybind11::is_method(this->c),
            pybind11::return_value_policy::reference_internal);

        // add auxiliary bindings
        visit([&](auto t) { add_aux_type(t, module); },
              std::make_tuple(
                  Type<typename std::decay<
                      typename ReturnValueConverter<Res&>::PyType>::type>{},
                  Type<typename std::decay<
                      typename ArgumentConverter<Res&>::AuxType>::type>{}));
        c.def_property_readonly(name, getter);
    }

    template <typename Res, typename Class>
    void op_impl(const char* name, Res(Class::*member_ptr),
                 std::false_type /* is_const */) const
    {
        REFLECT_LIB_DBUG("  readwrite property:", name, "of type",
                         demangle<Res>());

        // get reference Res&
        auto getter = pybind11::cpp_function(
            [member_ptr](Class & c) ->
            typename ReturnValueConverter<Res&>::PyType {
                return ReturnValueConverter<Res&>::cpp2py(c.*member_ptr);
            },
            pybind11::is_method(this->c),
            pybind11::return_value_policy::reference_internal);

        // set concrete type Res
        std::string const name_(name);
        auto setter = [member_ptr, name_](
            Class& c, typename ArgumentConverter<Res>::PyType value) {
            REFLECT_LIB_DBUG("setting ", name_, "\n  Class: ", demangle<Class>(),
                 "\n  Res:   ", demangle<Res>(), "\n  PyType:",
                 demangle(
                     typeid(typename ArgumentConverter<Res>::PyType).name()));
            c.*member_ptr = ArgumentConverter<Res>::py2cpp(
                std::forward<typename ArgumentConverter<Res>::PyType>(value));
        };

        // add auxiliary bindings
        visit([&](auto t) { add_aux_type(t, module); },
              std::make_tuple(
                  Type<typename std::decay<
                      typename ReturnValueConverter<Res&>::PyType>::type>{},
                  Type<typename std::decay<
                      typename ArgumentConverter<Res>::AuxType>::type>{}));
        c.def_property(name, getter, setter);
    }
};

template <typename Class, typename... Args>
DefFieldsVisitor<Class> makeDefFieldsVisitor(Class& c, Args&&... args)
{
    return DefFieldsVisitor<Class>{c, std::forward<Args>(args)...};
}

template <typename PybindClass>
struct DefMethodsVisitor {
    PybindClass& c;
    Module& module;

    template <typename MemberFctPtr>
    void operator()(
        std::pair<const char*, MemberFctPtr> const& name_member) const
    {
        op_impl(name_member.first, name_member.second, name_member.second);
    }

private:
    template <typename MemberFctPtr, typename Res, typename Class,
              typename... Args>
    void op_impl(const char* name, MemberFctPtr member_ptr,
                 Res (Class::*)(Args...) const) const
    {
        op_impl(name, member_ptr,
                static_cast<Res (Class::*)(Args...)>(nullptr));
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
            return ReturnValueConverter<Res>::cpp2py(
                (c.*member_ptr)(ArgumentConverter<Args>::py2cpp(
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
            (c.*member_ptr)(ArgumentConverter<Args>::py2cpp(
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
                      typename ArgumentConverter<Args>::AuxType>::type>{}...));
        c.def(name, wrapped_method, policy);
    }
};

template <typename Class, typename... Args>
DefMethodsVisitor<Class> makeDefMethodsVisitor(Class& c, Args&&... args)
{
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
    REFLECT_LIB_DBUG("namespace and name:", *namespace_name, " :: ", name);

    // add constructor
    detail::add_ctor(c);
    detail::add_ctor(c, Class::Meta::fields(), *this);

    // add data fields
    visit(detail::makeDefFieldsVisitor(c, *this), Class::Meta::fields());

    // add methods
    visit(detail::makeDefMethodsVisitor(c, *this), Class::Meta::methods());

    detail::add_pickling(c, Class::Meta::fields());

    return c;
}

#define CONCAT(a, b, c) CONCAT_(a, b, c)
#define CONCAT_(a, b, c) a##b##c

#define REFLECT_LIB_PYTHON_MODULE(name, variable) \
    APPLY(PYBIND11_MODULE,                        \
          CONCAT(REFLECT_LIB_PYTHON_MODULE_NAME_PREFIX, __, name), variable)

}  // namespace reflect_lib
