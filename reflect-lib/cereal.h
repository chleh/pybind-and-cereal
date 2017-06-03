#pragma once

#if 0

#define REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(CLASS, BASE)

#else

#include <utility>
#include <type_traits>

#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp> // TODO also add other

#include "reflect-macros.h"

#define REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(CLASS, BASE) \
    APPLY(CEREAL_REGISTER_TYPE, EXPAND(CLASS)); \
    OVERRIDE_CEREAL_REGISTER_POLYMORPHIC_RELATION(BASE, CLASS)

#define OVERRIDE_CEREAL_REGISTER_POLYMORPHIC_RELATION(Base, Derived)                     \
  namespace cereal {                                                            \
  namespace detail {                                                            \
  template <>                                                                   \
  struct PolymorphicRelation<EXPAND(Base), EXPAND(Derived)>                                     \
  { static void bind() { RegisterPolymorphicCaster<EXPAND(Base), EXPAND(Derived)>::bind(); } }; \
  } } /* end namespaces */

template <class Archive, class Object>
void save(Archive & archive, Object const& obj)
{
    save_impl(archive, obj, Object::Meta::fields(), std::is_same<typename Object::Meta::base, void>{});
}

template <class Archive, class Object, class... Pairs>
void save_impl(Archive & archive, Object const& obj, std::tuple<Pairs...>&& fields, std::true_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    save_impl_no_base(archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs>
void save_impl(Archive & archive, Object const& obj, std::tuple<Pairs...>&& fields, std::false_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    save_impl_derived(archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void save_impl_no_base(Archive & archive, Object const& obj, std::tuple<Pairs...>&& fields, std::index_sequence<Idcs...>)
{
    archive(cereal::make_nvp(std::get<Idcs>(fields).first, obj.*std::get<Idcs>(fields).second) ...);
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void save_impl_derived(Archive & archive, Object const& obj, std::tuple<Pairs...>&& fields, std::index_sequence<Idcs...>)
{
    archive(cereal::base_class<typename Object::Meta::base>(&obj),
            cereal::make_nvp(std::get<Idcs>(fields).first, obj.*std::get<Idcs>(fields).second) ...);
}



template <class Archive, class Object>
void load(Archive & archive, Object& obj)
{
    load_impl(archive, obj, Object::Meta::fields(), std::is_same<typename Object::Meta::base, void>{});
}

template <class Archive, class Object, class... Pairs>
void load_impl(Archive & archive, Object& obj, std::tuple<Pairs...>&& fields, std::true_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    load_impl_no_base(archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs>
void load_impl(Archive & archive, Object& obj, std::tuple<Pairs...>&& fields, std::false_type)
{
    using Idcs = std::index_sequence_for<Pairs...>;
    load_impl_derived(archive, obj, std::forward<std::tuple<Pairs...>>(fields), Idcs{});
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void load_impl_no_base(Archive & archive, Object& obj, std::tuple<Pairs...>&& fields, std::index_sequence<Idcs...>)
{
    archive(cereal::make_nvp(std::get<Idcs>(fields).first, obj.*std::get<Idcs>(fields).second) ...);
}

template <class Archive, class Object, class... Pairs, std::size_t... Idcs>
void load_impl_derived(Archive & archive, Object& obj, std::tuple<Pairs...>&& fields, std::index_sequence<Idcs...>)
{
    archive(cereal::base_class<typename Object::Meta::base>(&obj),
            cereal::make_nvp(std::get<Idcs>(fields).first, obj.*std::get<Idcs>(fields).second) ...);
}

#endif
