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

    return 0;
}
