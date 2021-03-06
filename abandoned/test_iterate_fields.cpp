/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <cassert>
#include <iostream>
#include <type_traits>

#include "reflect-lib/remangle.h"
#include "reflect-lib/util.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"

template <typename T>
void output(T const& obj)
{
    std::cout << "output called\n";
    reflect_lib::visit(
        [&obj](auto const& field_info) {
            auto const& field_name = field_info.first;
            auto const& field_accessor = field_info.second;
            auto const& value = obj.*field_accessor;
            std::cout << "  visiting field `" << field_name << "' of type `"
                      << reflect_lib::demangle(
                             // TODO decay might decay too much. Instead infer
                             // type from field_accessor directly
                             typeid(std::decay_t<decltype(value)>).name())
                      << "' with value " /*<< value << '\n'*/
                         "[TODO IMPLEMENT FEATURE]\n";
        },
        T::Meta::fields());
}

template <typename T>
void output(std::unique_ptr<T> const& obj)
{
    std::cout << "unique_ptr output called\n";
    if (obj)
        output(*obj);
    else
        std::cout << "nullptr of type "
                  << reflect_lib::demangle(typeid(T).name()) << '\n';
}

class FieldInfoFilter
{
    template <typename T>
    void operator()()
    {
    }
};

template <typename T, typename Key, typename... Keys>
decltype(auto) getValue(T const& obj, Key&& key, Keys&&... keys)
{
    std::string key_s(key);
    auto accessors = reflect_lib::apply(
        [&obj, &key_s](auto const& field_info) {
            if (field_info.first == key_s) {
                return field_info.first;
            } else {
                return (char const*)nullptr;
            }
        },
        T::Meta::fields());
    for (auto a : accessors) {
        if (a)
            return a;
    }

    return (typename std::decay<decltype(accessors[0])>::type) nullptr;
}

template <typename T, typename D, typename Key, typename... Keys>
decltype(auto) getValue(std::unique_ptr<T, D> const& obj,
                        Key&& key,
                        Keys&&... keys)
{
    return getValue(*obj, std::forward<Key>(key), std::forward<Keys>(keys)...);
}

class GetValue
{
public:
    virtual ~GetValue() = default;

    virtual double getDouble(std::vector<std::string> path) const = 0;
};

template <typename T>
class GeneralGetValue : public GetValue
{
public:
    explicit GeneralGetValue(T const& obj) : obj_(obj) {}

    double getDouble(std::vector<std::string> path) const override
    {
        return 0.0;
    }

private:
    T const& obj_;
};

template <typename T>
std::unique_ptr<GetValue> makeGetValue(T const& obj)
{
    return std::make_unique<GeneralGetValue<T>>(obj);
}

template <typename P, typename D>
std::unique_ptr<GetValue> makeGetValue(std::unique_ptr<P, D> const& obj)
{
    if (obj)
        return makeGetValue(*obj);
    return nullptr;
}



class GetFieldPredicate
{
public:
    explicit GetFieldPredicate(std::string const& field_name)
        : field_name_{field_name}
    {
    }

    template <typename P>
    bool operator()(P const& p)
    {
        // std::cout << "GetDoubleFieldPredicate::op(): " << p.first << '\n';
        return p.first == field_name_;
    }

private:
    std::string const field_name_;
};


class GetDoubleFieldPredicate
{
public:
    explicit GetDoubleFieldPredicate(std::string const& field_name)
        : field_name_{field_name}
    {
    }

    template <typename O>
    bool operator()(std::pair<const char*, double O::*> const& p)
    {
        // std::cout << "GetDoubleFieldPredicate::op(): " << p.first << '\n';
        return p.first == field_name_;
    }

private:
    std::string const field_name_;
};

template <typename Function>
decltype(auto) get_first_incl_ancestors(Function&& fct, Type<void>)
{
    return nullptr;
}

template <typename A, typename B>
struct NotNull {
    static_assert(std::is_same<A, B>::value, "Types differ");
};
template <typename A>
struct NotNull<A, std::nullptr_t> {
    using type = A;
};
template <typename A>
struct NotNull<std::nullptr_t, A> {
    using type = A;
};
template <>
struct NotNull<std::nullptr_t, std::nullptr_t> {
    using type = std::nullptr_t;
};

template <typename Function, typename Class>
auto get_first_incl_ancestors(Function&& fct, Type<Class>) -> typename NotNull<
    decltype(get_first_incl_ancestors(
        std::declval<Function>(),
        std::declval<Type<typename Class::Meta::base>>())),
    decltype(reflect_lib::get_first(
        std::declval<Function>(),
        std::declval<decltype(Class::Meta::fields())>()))>::type
{
    auto res = get_first_incl_ancestors(std::forward<Function>(fct),
                                        Type<typename Class::Meta::base>{});
    if (res != nullptr) {
        return res;
    }

    return reflect_lib::get_first(fct, Class::Meta::fields());
}

template <typename Class, typename Function>
decltype(auto) get_first_incl_ancestors(Function&& fct)
{
    return get_first_incl_ancestors(std::forward<Function>(fct), Type<Class>{});
}

int main()
{
    auto d1 =
        std::make_unique<types_one::types_one_a::types_one_a_a::Derived1>();
    d1->i = 10;
    d1->d = 3.14;
    d1->s = "hello";

    auto d2 =
        std::make_unique<types_one::types_one_a::types_one_a_a::Derived1>();
    d2->i = 5;
    d2->d = 2.71;
    d2->s = "bye";

    d1->b = std::move(d2);

    output(d1);

    std::cout << "get value: `" << getValue(d1, "b", "d") << "'\n";

    auto const getter = makeGetValue(d1);
    std::cout << "get value: `" << getter->getDouble({"b", "d"}) << "'\n";

    auto ptr = get_first_incl_ancestors<
        types_one::types_one_a::types_one_a_a::Derived1>(
        GetDoubleFieldPredicate{"d"});

    assert(ptr);
    assert((*d1).*(ptr->second) == 3.14);


#if 0
    auto ptr2 = get_first_incl_ancestors<
        types_one::types_one_a::types_one_a_a::Derived1>(
        GetFieldPredicate{"d"});

    assert(ptr2);
#endif

    return 0;
}
