#pragma once

#include <type_traits>

#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>

#include "cereal-impl.h"
#include "reflect-macros.h"

#define REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(CLASS, BASE) \
    APPLY(CEREAL_REGISTER_TYPE, EXPAND(CLASS)); \
    OVERRIDE_CEREAL_REGISTER_POLYMORPHIC_RELATION(BASE, CLASS)

#define OVERRIDE_CEREAL_REGISTER_POLYMORPHIC_RELATION(Base, Derived)          \
    namespace cereal                                                          \
    {                                                                         \
    namespace detail                                                          \
    {                                                                         \
    template <>                                                               \
    struct PolymorphicRelation<EXPAND(Base), EXPAND(Derived)> {               \
        static void bind()                                                    \
        {                                                                     \
            RegisterPolymorphicCaster<EXPAND(Base), EXPAND(Derived)>::bind(); \
        }                                                                     \
    };                                                                        \
    }                                                                         \
    } /* end namespaces */

// Must be used exactly once in every namespace containing types
// to be serialized.
#define DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS                               \
    template <class Archive, class Object>                              \
    void CEREAL_SAVE_FUNCTION_NAME(Archive& archive, Object const& obj) \
    {                                                                   \
        reflect_lib::detail::save_impl(                                 \
            archive, obj, Object::Meta::fields(),                       \
            std::is_same<typename Object::Meta::base, void>{});         \
    }                                                                   \
                                                                        \
    template <class Archive, class Object>                              \
    void CEREAL_LOAD_FUNCTION_NAME(Archive& archive, Object& obj)       \
    {                                                                   \
        reflect_lib::detail::load_impl(                                 \
            archive, obj, Object::Meta::fields(),                       \
            std::is_same<typename Object::Meta::base, void>{});         \
    }
