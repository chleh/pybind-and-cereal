#include <tuple>
#include <utility>
#include <vector>

namespace reflect_lib
{
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

}  // namespace reflect_lib
