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

#include "cereal-impl.h"

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

// namespace reflect_lib
// {

template <class Archive, class Object>
void save(Archive & archive, Object const& obj)
{
    detail::save_impl(archive, obj, Object::Meta::fields(), std::is_same<typename Object::Meta::base, void>{});
}

template <class Archive, class Object>
void load(Archive & archive, Object& obj)
{
    detail::load_impl(archive, obj, Object::Meta::fields(), std::is_same<typename Object::Meta::base, void>{});
}

// }  // namespace reflect_lib

#endif
