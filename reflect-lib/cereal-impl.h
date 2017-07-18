#pragma once

#include <tuple>
#include <utility>

#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp> // TODO also add other

namespace reflect_lib
{
namespace detail
{
template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void save_impl_no_base(Archive& archive,
                       Object const& obj,
                       std::tuple<Pairs...>&& fields,
                       std::index_sequence<Idcs...>)
{
    archive(cereal::make_nvp(std::get<Idcs>(fields).first,
                             obj.*std::get<Idcs>(fields).second)...);
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void save_impl_derived(Archive& archive,
                       Object const& obj,
                       std::tuple<Pairs...>&& fields,
                       std::index_sequence<Idcs...>)
{
    archive(cereal::base_class<typename Object::Meta::base>(&obj),
            cereal::make_nvp(std::get<Idcs>(fields).first,
                             obj.*std::get<Idcs>(fields).second)...);
}

template <class Archive, class Object, class... Pairs>
void save_impl(Archive& archive,
               Object const& obj,
               std::tuple<Pairs...>&& fields,
               std::true_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    save_impl_no_base(
        archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs>
void save_impl(Archive& archive,
               Object const& obj,
               std::tuple<Pairs...>&& fields,
               std::false_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    save_impl_derived(
        archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void load_impl_no_base(Archive& archive,
                       Object& obj,
                       std::tuple<Pairs...>&& fields,
                       std::index_sequence<Idcs...>)
{
    archive(cereal::make_nvp(std::get<Idcs>(fields).first,
                             obj.*std::get<Idcs>(fields).second)...);
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void load_impl_derived(Archive& archive,
                       Object& obj,
                       std::tuple<Pairs...>&& fields,
                       std::index_sequence<Idcs...>)
{
    archive(cereal::base_class<typename Object::Meta::base>(&obj),
            cereal::make_nvp(std::get<Idcs>(fields).first,
                             obj.*std::get<Idcs>(fields).second)...);
}

template <class Archive, class Object, class... Pairs>
void load_impl(Archive& archive,
               Object& obj,
               std::tuple<Pairs...>&& fields,
               std::true_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    load_impl_no_base(
        archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs>
void load_impl(Archive& archive,
               Object& obj,
               std::tuple<Pairs...>&& fields,
               std::false_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    load_impl_derived(
        archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

}  // namespace detail
}  // namespace reflect_lib
