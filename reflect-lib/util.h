#include <tuple>
#include <utility>
#include <vector>

namespace reflect_lib
{

template<typename T>
struct ResultType;

template<typename Res, typename Class>
struct ResultType<Res Class::*>
{
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
    std::vector<Result> result{ v(std::forward<Ts>(std::get<Idcs>(t)))... };
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

template <typename T>
union PoorMansOptionalValue
{
    PoorMansOptionalValue() = default;
    PoorMansOptionalValue(T const& t) : value{t} {}
    // char dummy;
    T value;
};


// optional values ////////////////////////////////////////////////////////////
template <typename T>
using PoorMansOptional = std::pair<bool, PoorMansOptionalValue<T>>;

template <typename T>
PoorMansOptional<T> makeEmptyOptional()
{
    return {false, PoorMansOptionalValue<T>{}};
}

template <typename Result, typename Visitor>
std::pair<bool, PoorMansOptionalValue<Result>> get_first(Visitor&& v,
                                                    std::tuple<>&& t)
{
    return makeEmptyOptional<T>();
}
// optional values end ////////////////////////////////////////////////////////

template <typename Result, typename Visitor>
PoorMansOptional get_first(Visitor&& v, std::tuple<>&& t)
{
    return makeEmptyOptional<Result>();
}

template <typename Result, typename Visitor, typename T, typename... Ts>
PoorMansOptional get_first(Visitor&& v, std::tuple<T, Ts...>&& t)
{
    auto res = v(std::forward<T>(std::get<0>(t)));
    if (res.first)
        return res;

    return get_first(std::forward<Visitor>(v), std::get<Indices...>(t));
}


}  // namespace reflect_lib
