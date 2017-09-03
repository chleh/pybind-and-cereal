#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <iostream>
#include <typeinfo>

#include "reflect-lib/remangle.h"

namespace reflect_lib
{
template <typename T>
struct ResultType;

template <typename Res, typename Class>
struct ResultType<Res Class::*> {
    using type = Res;
};

template <typename Res, typename Class, typename... Args>
struct ResultType<Res Class::*(Args...)> {
    using type = Res;
};

template <typename Res, typename... Args>
struct ResultType<Res(Args...)> {
    using type = Res;
};

namespace detail
{
struct NoOp {
    template <typename... Ts>
    NoOp(Ts&&...)
    {
    }
};

template <typename Visitor, typename... Ts, std::size_t... Idcs>
void visit_impl(Visitor&& v, std::tuple<Ts...>&& t,
                std::index_sequence<Idcs...>)
{
    NoOp{(v(std::forward<Ts>(std::get<Idcs>(t))), 0)...};
}

template <typename Visitor, typename... Ts, std::size_t... Idcs>
decltype(auto) apply_impl(Visitor&& v, std::tuple<Ts...>&& t,
                          std::index_sequence<Idcs...>)
{
    using Result = decltype(v(std::get<0>(t)));
    // TODO use different type!
    std::vector<Result> result{v(std::forward<Ts>(std::get<Idcs>(t)))...};
    return result;
}

}  // namespace detail

template <typename Visitor, typename... Ts>
void visit(Visitor&& v, std::tuple<Ts...>&& t)
{
    using Idcs = std::index_sequence_for<Ts...>;
    detail::visit_impl(std::forward<Visitor>(v),
                       std::forward<std::tuple<Ts...>>(t), Idcs{});
}

template <typename Visitor, typename... Ts>
decltype(auto) apply(Visitor&& v, std::tuple<Ts...>&& t)
{
    using Idcs = std::index_sequence_for<Ts...>;
    return detail::apply_impl(std::forward<Visitor>(v),
                              std::forward<std::tuple<Ts...>>(t), Idcs{});
}

namespace detail
{
template <typename Visitor>
std::nullptr_t get_first(Visitor&&, std::tuple<>&&)
{
    return nullptr;
}

template <typename Predicate, typename... Ts>
std::nullptr_t get_first_impl2(Predicate&&)
{
    return nullptr;
}

#if 0
template <typename Predicate, typename T, typename... Ts>
// decltype(std::declval<Predicate>()(std::declval<T>()))
decltype(auto) get_first_impl2(
    Predicate&& p, T&& obj, Ts&&... objs)
{
    auto match = p(std::forward<T>(obj));
    if (match)
        return &obj;
    return get_first_impl2(std::forward<Predicate>(p),
                           std::forward<Ts>(objs)...);
}
#endif

// Cf. http://stackoverflow.com/a/16824239

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.
template <typename, typename T>
struct HasSuitableCallOperator {
    static_assert(std::integral_constant<T, false>::value,
                  "Second template parameter needs to be of function type.");
};

// specialization that does the checking
template <typename C, typename Ret, typename... Args>
struct HasSuitableCallOperator<C, Ret(Args...)> {
private:
    template <typename T>
    static constexpr auto check(T*) -> typename std::is_same<
        decltype(std::declval<T>()(std::declval<Args>()...)),
        Ret       // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;  // attempt to call it and see if the return type is correct

    template <typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(nullptr)) type;

public:
    static constexpr bool value = type::value;
};

template <typename C, typename Ret, typename... Args>
struct HasSuitableCallOperator<C&, Ret(Args...)>
    : public HasSuitableCallOperator<C, Ret(Args...)> {
};
template <typename C, typename Ret, typename... Args>
struct HasSuitableCallOperator<C const&, Ret(Args...)>
    : public HasSuitableCallOperator<C, Ret(Args...)> {
};
template <typename C, typename Ret, typename... Args>
struct HasSuitableCallOperator<C&&, Ret(Args...)>
    : public HasSuitableCallOperator<C, Ret(Args...)> {
};

template <typename Predicate, typename T, typename... Ts>
// decltype(std::declval<Predicate>()(std::declval<T>()))
decltype(auto) get_first_impl2(Predicate&& p, T&& obj, Ts&&... objs);

template <typename Predicate, typename T, typename... Ts>
// decltype(std::declval<Predicate>()(std::declval<T>()))
std::unique_ptr<T> get_first_impl3(std::true_type, Predicate&& p, T&& obj,
                                   Ts&&... objs)
{
    if (p(std::forward<T>(obj)))
        return std::unique_ptr<T>(new T(obj));
    return get_first_impl2(std::forward<Predicate>(p),
                           std::forward<Ts>(objs)...);
}

template <typename Predicate, typename T, typename... Ts>
// decltype(std::declval<Predicate>()(std::declval<T>()))
decltype(auto) get_first_impl3(std::false_type, Predicate&& p, T&& obj,
                               Ts&&... objs)
{
    return get_first_impl2(std::forward<Predicate>(p),
                           std::forward<Ts>(objs)...);
}

template <typename Predicate, typename T, typename... Ts>
// decltype(std::declval<Predicate>()(std::declval<T>()))
decltype(auto) get_first_impl2(Predicate&& p, T&& obj, Ts&&... objs)
{
#if 0
    std::cout << "get_first_impl2: " << demangle(typeid(obj).name()) << '\n';
    std::cout << "  Suit op()? " << demangle(typeid(Predicate).name()) << " "
              << HasSuitableCallOperator<Predicate, bool(T)>::value << '\n';
#endif
    return get_first_impl3(
        std::integral_constant<
            bool, HasSuitableCallOperator<Predicate, bool(T)>::value>{},
        std::forward<Predicate>(p),
        std::forward<T>(obj),
        std::forward<Ts>(objs)...);
}

template <typename Predicate, typename... Ts, std::size_t... Indices>
decltype(auto) get_first_impl(Predicate&& p, std::tuple<Ts...>&& t,
                              std::index_sequence<Indices...>)
{
    return get_first_impl2(std::forward<Predicate>(p),
                           std::forward<Ts>(std::get<Indices>(t))...);
}

template <typename T>
struct IsCopyConstructible : std::is_copy_constructible<T> {
};

template <typename T, typename A>
struct IsCopyConstructible<std::vector<T, A>> : IsCopyConstructible<T> {
};

}  // namespace detail

template <typename Predicate, typename... Ts>
decltype(auto) get_first(Predicate&& p, std::tuple<Ts...>&& t)
{
    using Indices = std::index_sequence_for<Ts...>;
    return detail::get_first_impl(std::forward<Predicate>(p),
                                  std::forward<std::tuple<Ts...>>(t),
                                  Indices{});
}

}  // namespace reflect_lib
