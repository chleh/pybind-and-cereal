/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <typeinfo>

#include <pybind11/pybind11.h>

#include "reflect-lib/remangle.h"

#define TO_STRING_(x) #x
#define TO_STRING(x) TO_STRING_(x)
#define REFLECT_LIB_THROW(type, message) \
    throw type(std::string(__FILE__ ":" TO_STRING(__LINE__) ": ") + message)

#define REFLECT_LIB_DBUG(...) \
    pybind11::print(__VA_ARGS__, "\t\t(" __FILE__ ":" TO_STRING(__LINE__) ")")

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

namespace detail
{
template <typename T>
struct IsCopyConstructible : std::is_copy_constructible<T> {
};

template <typename T, typename A>
struct IsCopyConstructible<std::vector<T, A>> : IsCopyConstructible<T> {
};

}  // namespace detail
}  // namespace reflect_lib
