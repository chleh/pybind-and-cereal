#pragma once

#include "aux-types.h"
#include "util.h"

namespace reflect_lib
{
namespace detail
{
// argument converters /////////////////////////////////////////////////////////

// copy constructible CPPType_
template <typename CPPType_, bool IsCopyConstructible>
struct ArgumentConverterImpl {
    using CPPType = CPPType_ const&;
    using PyType = CPPType_ const&;
    using AuxType = PyType;

// TODO why does it work without the overloads?
#if 0
    // static CPPType_&& py2cpp(PyType&& o) { return std::move(o); }
    template <typename T>
    static CPPType_&& py2cpp(
        typename std::decay<T>::type&& o,
        typename std::enable_if<!std::is_same<T, PyType const>::value,
                                PyType&>::type* = nullptr)
    {
        std::cout << "conversion &&! " << demangle(typeid(PyType).name())
                  << '\n';
        return o;
    }
    template <typename T>
    static CPPType_& py2cpp(
        typename std::decay<T>::type& o,
        typename std::enable_if<!std::is_same<T, PyType const>::value,
                                PyType&>::type* = nullptr)
    {
        std::cout << "conversion &! " << demangle(typeid(PyType).name())
                  << '\n';
        return o;
    }
#endif
    static CPPType py2cpp(PyType o)
    {
        // std::cout << "conversion const&! " << demangle(typeid(PyType).name())
        //           << '\n';
        return o;
    }
};

// TODO: type that is neither copyable nor movable
// not copy constructible CPPType_
template <typename CPPType_>
struct ArgumentConverterImpl<CPPType_, false /* IsCopyConstructible */> {
    using CPPType = CPPType_&&;
    using PyType = RValueReference<CPPType_>&;
    using AuxType = PyType;

    static CPPType py2cpp(PyType o) { return o.get(); }
    static CPPType py2cpp(RValueReference<CPPType_>&& o) { return o.get(); }
};

template <typename CPPType_>
struct ArgumentConverter
    : ArgumentConverterImpl<CPPType_, IsCopyConstructible<CPPType_>::value> {
};

template <typename CPPType_>
struct ArgumentConverter<CPPType_&> {
    using CPPType = CPPType_&;
    using PyType = CPPType;
    using AuxType = PyType;

    static CPPType py2cpp(PyType o) { return o; }
};

template <typename CPPType_>
struct ArgumentConverter<CPPType_ const&> {
    using CPPType = CPPType_ const&;
    using PyType = CPPType;
    using AuxType = PyType;

    static CPPType py2cpp(PyType o) { return o; }
};

template <typename CPPType_>
struct ArgumentConverter<CPPType_&&> {
    using CPPType = CPPType_&&;

    // TODO static_assert that this is reference type? (for all
    // ArgumentConverter types)
    using PyType = RValueReference<CPPType_>&;
    using AuxType = PyType;

    static CPPType py2cpp(PyType o) { return o.get(); }
};

template <typename P, typename D>
static std::unique_ptr<P, D> none = std::unique_ptr<P, D>{};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>> {
    using CPPType = std::unique_ptr<P, D>&&;
    using PyType = pybind11::object;
    using AuxType = UniquePtrReference<P>;

    static CPPType py2cpp(PyType o)
    {
        if (o.is_none()) {
            none<P, D>.reset();
            return std::move(none<P, D>);
        }
        try {
            auto* p = o.cast<UniquePtrReference<P>*>();
            return p->getRValue();
        } catch (pybind11::cast_error) {
        }
        // TODO better error message
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
    static CPPType py2cpp(AuxType&& o) { return o.getRValue(); }
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>&&> {
    using CPPType = std::unique_ptr<P, D>&&;
    using PyType = pybind11::object;
    using AuxType = UniquePtrReference<P>;

    static CPPType py2cpp(PyType o)
    {
        if (o.is_none()) {
            none<P, D>.reset();
            return std::move(none<P, D>);
        }
        try {
            auto* p = o.cast<UniquePtrReference<P>*>();
            return p->getRValue();
        } catch (pybind11::cast_error) {
        }
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D>&> {
    using CPPType = std::unique_ptr<P, D>&;
    using PyType = pybind11::object;
    using AuxType = UniquePtrReference<P>;

    static CPPType py2cpp(PyType o)
    {
        if (o.is_none()) {
            none<P, D>.reset();
            return none<P, D>;
        }
        try {
            auto* p = o.cast<UniquePtrReference<P>*>();
            return p->get();
        } catch (pybind11::cast_error) {
        }
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

template <typename P, typename D>
struct ArgumentConverter<std::unique_ptr<P, D> const&> {
    using CPPType = std::unique_ptr<P, D> const&;
    using PyType = pybind11::object;
    using AuxType = UniquePtrReference<P>;

    static CPPType py2cpp(PyType o)
    {
        if (o.is_none()) {
            none<P, D>.reset();
            return none<P, D>;
        }
        try {
            auto* p = o.cast<UniquePtrReference<P>*>();
            return p->getConst();
        } catch (pybind11::cast_error) {
        }
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

template <typename VecElem, typename VecAlloc>
struct ArgumentConverter<std::vector<VecElem, VecAlloc>> {
    using CPPType = std::vector<VecElem, VecAlloc>;
    using PyType = pybind11::object&;
    using AuxType = CPPType;

    static CPPType py2cpp(PyType o)
    {
        using Vec = std::vector<VecElem, VecAlloc>;
        try {
            auto& p = o.cast<Vec&>();
            return p;
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to " << demangle(typeid(Vec).name())
                      << std::endl;
        }
        try {
            auto it = o.cast<pybind11::iterable>();
            Vec v;
            for (auto& e : it)
                v.emplace_back(e.cast<VecElem>());
            return v;
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to pybind11::iterable\n";
        }

        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

template <typename T, typename VecAlloc>
struct ArgumentConverter<std::vector<std::shared_ptr<T>, VecAlloc>> {
    using CPPType = std::vector<std::shared_ptr<T>, VecAlloc>;
    using PyType = pybind11::object&;
    using AuxType = CPPType;

    static CPPType py2cpp(PyType o)
    {
        using Vec = CPPType;
        try {
            auto& p = o.cast<Vec&>();
            return p;
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to " << demangle(typeid(Vec).name())
                      << std::endl;
        }
        try {
            auto it = o.cast<pybind11::iterable>();
            Vec v;
            for (auto& e : it)
                v.emplace_back(e.cast<smart_ptr<T>>().new_copied());
            return v;
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to pybind11::iterable\n";
        }

        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

template <typename T, typename VecAlloc>
struct ArgumentConverter<std::vector<std::unique_ptr<T>, VecAlloc>> {
    using CPPType = std::vector<std::unique_ptr<T>, VecAlloc>;
    using PyType = pybind11::object&;
    using AuxType = CPPType;

    static CPPType py2cpp(PyType o)
    {
        using Vec = CPPType;
        try {
            auto& p = o.cast<RValueReference<Vec>&>();
            return Vec{p.get()};
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to " << demangle(typeid(Vec).name())
                      << std::endl;
        }
        try {
            auto it = o.cast<pybind11::iterable>();
            Vec v;
            for (auto& e : it)
                v.emplace_back(e.cast<smart_ptr<T>>().new_copied());
            return v;
        } catch (pybind11::cast_error e) {
            std::cout << "  ERR: " << e.what() << '\n';
            std::cout << "  could not cast to pybind11::iterable\n";
        }

        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

// end argument converters /////////////////////////////////////////////////////

// return value converters /////////////////////////////////////////////////////

template <typename CPPType>
struct ReturnValueConverter {
private:
    using CPPTypeNoRef =
        std::decay_t<CPPType>;  // std::remove_reference_t<CPPType>;

public:
    static CPPTypeNoRef&& cpp2py(CPPTypeNoRef&& o)
    {
        return std::forward<CPPType>(o);
    }
    static CPPTypeNoRef const& cpp2py(CPPTypeNoRef const& o) { return o; }
    static CPPTypeNoRef& cpp2py(CPPTypeNoRef& o) { return o; }

    using PyType = CPPType;
};

template <>
struct ReturnValueConverter<void> {
    using PyType = void;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>> {
    static smart_ptr<P> cpp2py(std::unique_ptr<P, D> p)
    {
        // TODO: might create wrong copy/move function!
        return smart_ptr<P>(p.release());
    }

    using PyType = typename ResultType<decltype(cpp2py)>::type;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>&&> {
    static std::unique_ptr<P, D> const& cpp2py(std::unique_ptr<P, D>&& p)
    {
        // TODO: might create wrong copy/move function!
        return smart_ptr<P>(p.release());
    }

    using PyType = typename ResultType<decltype(cpp2py)>::type;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D>&> {
    static P* cpp2py(std::unique_ptr<P, D>& p) { return p.get(); }

    // TODO add return value policy!
    using PyType = P*;
};

template <typename P, typename D>
struct ReturnValueConverter<std::unique_ptr<P, D> const&> {
    static P* cpp2py(std::unique_ptr<P, D> const& p) { return p.get(); }

    using PyType = P*;
};

// end return value converters /////////////////////////////////////////////////

// TODO reduce duplicate code
// pickle converters ///////////////////////////////////////////////////////////

template <typename CPPType>
struct PickleConverter {
    static CPPType const* cpp2py(CPPType const& o) { return &o; }
};

template <typename P, typename D>
struct PickleConverter<std::unique_ptr<P, D>> {
    static P const* cpp2py(std::unique_ptr<P, D> const& p) { return p.get(); }
};

template <typename P>
struct PickleConverter<std::shared_ptr<P>> {
    static P const* cpp2py(std::shared_ptr<P> const& p) { return p.get(); }
};

// end pickle converters ///////////////////////////////////////////////////////

// TODO reduce duplicate code
// unpickle converters /////////////////////////////////////////////////////////

// copy constructible CPPType_
template <typename CPPType_, bool IsCopyConstructible>
struct UnpickleConverterImpl {
    using CPPType = CPPType_;
    using PyType = pybind11::object;
    using AuxType = PyType;

    static CPPType py2cpp(PyType o)
    {
        // std::cout << "conversion const&! " << demangle(typeid(PyType).name())
        //           << '\n';
        return o.cast<CPPType>();
    }
};

// TODO: type that is neither copyable nor movable
// not copy constructible CPPType_
template <typename CPPType_>
struct UnpickleConverterImpl<CPPType_, false> {
    using CPPType = CPPType_;
    using PyType = pybind11::object;
    using AuxType = CPPType_;

    static CPPType py2cpp(PyType o) { return std::move(o).cast<CPPType>(); }
    // static CPPType py2cpp(RValueReference<CPPType_>&& o) { return o.get(); }
};

template <typename CPPType_>
struct UnpickleConverter
    : UnpickleConverterImpl<CPPType_, IsCopyConstructible<CPPType_>::value> {
    static_assert(!std::is_reference<CPPType_>::value,
                  "Reference types not supported by this class template");
};

template <typename P, typename D>
struct UnpickleConverter<std::unique_ptr<P, D>> {
    using CPPType = std::unique_ptr<P, D>;
    using PyType = pybind11::object;
    using AuxType = UniquePtrReference<P>;

    static CPPType py2cpp(PyType o)
    {
        if (o.is_none()) {
            none<P, D>.reset();
            return std::move(none<P, D>);
        }
        try {
            auto p = o.cast<smart_ptr<P>>();
            return std::unique_ptr<P, D>(p.release());
        } catch (pybind11::cast_error e) {
            std::cout << "Error: " << e.what() << '\n';
        }
        // TODO better error message
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
    static CPPType py2cpp(AuxType&& o) { return o.getRValue(); }
};

template <typename P, typename D>
struct UnpickleConverter<std::vector<std::unique_ptr<P, D>>> {
    using CPPType = std::vector<std::unique_ptr<P, D>>;
    using PyType = pybind11::object;
    using AuxType = CPPType;

    static CPPType py2cpp(PyType o)
    {
        try {
            auto l = o.cast<pybind11::list>();
            CPPType vec;
            vec.reserve(pybind11::len(l));
            for (auto& e : l) {
                // TODO: check if inc_ref() is correct
                e.inc_ref(); // Python should not destroy the object.
                vec.emplace_back(e.cast<P*>());
            }
            return vec;
        } catch (pybind11::cast_error e) {
            std::cout << "Error: " << e.what() << std::endl;
        } catch (std::exception e) {
            std::cout << "Exc. Error: " << e.what() << '\n';
        } catch (...) {
            std::cout << "OTHER EXC!" << std::endl;
        }

        // TODO better error message
        REFLECT_LIB_THROW(pybind11::type_error, "ERR.");
    }
};

// end unpickle converters /////////////////////////////////////////////////////

}  // namespace detail

}  // namespace reflect_lib
