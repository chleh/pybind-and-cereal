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

class GetDoubleFieldPredicate
{
public:
#if 0
    template <typename O>
    bool operator()(
        std::pair<const char*, types_one::types_one_a::NoCopy O::*> const& p)
    {
        std::cout << "GetDoubleFieldPredicate::op(): " << p.first << '\n';
        return true;
    }
#endif
    template <typename O>
    bool operator()(
        std::pair<const char*, double O::*> const& p)
    {
        std::cout << "GetDoubleFieldPredicate::op(): " << p.first << '\n';
        return true;
    }

#if 0
    template <typename T>
    bool operator()(std::pair<const char*, T>)
    {
        return false;
    }
#endif
};

#if 0
template <typename Functor, typename Function>
decltype(auto) apply_incl_ancestors(Functor&& ftor, Function&& fct, Type<void>)
{
    return nullptr;
}

template <typename Functor, typename Function, typename Class>
decltype(auto) apply_incl_ancestors(Functor&& ftor, Function&& fct, Type<Class>)
{
    auto res = apply_incl_ancestors(std::forward<Functor>(ftor),
                                    std::forward<Function>(fct),
                                    Type<typename Class::Meta::base>{});
    if (res)
        return res;
    return ftor(fct, Class::Meta::fields());
}

template <typename Class, typename Functor, typename Function>
decltype(auto) apply_incl_ancestors(Functor&& ftor, Function&& fct)
{
    return apply_incl_ancestors(std::forward<Functor>(ftor),
                                std::forward<Function>(fct),
                                Type<Class>{});
}
#endif

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
        // std::cout << "Res! " << (void*)res << '\n';
        return res;
    }
#if 0
    std::cout << "=========== " << reflect_lib::demangle(typeid(Class).name())
              << '\n';

    auto p = reflect_lib::get_first(
        std::move(fct),  // GetDoubleFieldPredicate{},
        types_one::types_one_a::types_one_a_a::Derived1::Meta::fields());
    std::cout << "== ZZZ  " << (void*)p << '\n';
    auto p2 = reflect_lib::get_first(
        GetDoubleFieldPredicate{},
        types_one::types_one_a::types_one_a_a::Derived1::Meta::fields());
    std::cout << "== ZZZ2 " << (void*)p2 << '\n';

#endif

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

    auto const ptr = reflect_lib::get_first(
        GetDoubleFieldPredicate{},
        types_one::types_one_a::types_one_a_a::Derived1::Meta::fields());

    std::cout << "XX " << (void*)ptr << '\n';

    auto ptr2 = get_first_incl_ancestors<
        types_one::types_one_a::types_one_a_a::Derived1>(
        GetDoubleFieldPredicate{});

    std::cout << "YY " << (void*)ptr2.get() << '\n';

    if (ptr2 && d1) {
        std::cout << "ptrs " << &types_one::Base::d << " " << ptr2->second << '\n';
        std::cout << "ptr int " << &types_one::Base::i << '\n';
        std::cout << "ptrs " << reflect_lib::demangle(typeid(&types_one::Base::d).name()) << " " << reflect_lib::demangle(typeid(ptr2->second).name()) << '\n';
        // assert(&types_one::Base::d == ptr2->second);
        auto p = &types_one::Base::d;
        std::cout << "((*d1).*p) " << ((*d1).*p) << '\n';

        std::cout << "YY name `" << ptr2->first << "'\n";
        std::cout << "YY ptr `" << ptr2->second << "'\n";
        std::cout << "d1 `" << d1.get() << "'\n";
        std::cout << reflect_lib::demangle(typeid(decltype(ptr2->second)).name()) << '\n';
        std::cout << reflect_lib::demangle(typeid(decltype(d1.get() ->* (ptr2->second))).name()) << '\n';
        types_one::Base* b = d1.get();
        std::cout << "b  `" << b << "'\n";
        std::cout << ((*b) .* (ptr2->second));
        std::cout << "YY value: `" << ((static_cast<types_one::Base&>(*d1)).*(ptr2->second)) << "'\n";
    }

    return 0;
}
