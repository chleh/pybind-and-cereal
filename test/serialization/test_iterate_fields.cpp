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


int main()
{
    auto d1 =
        std::make_unique<types_one::types_one_a::types_one_a_a::Derived1>();
    d1->i = 10;
    d1->d = 3.14;
    d1->s = "hello";

    auto d2 = std::make_unique<types_one::types_one_a::types_one_a_a::Derived1>();
    d2->i = 5;
    d2->d = 2.71;
    d2->s = "bye";

    d1->b = std::move(d2);

    output(d1);

    return 0;
}
