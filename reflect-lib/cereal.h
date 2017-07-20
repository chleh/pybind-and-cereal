#pragma once

#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>

#include "cereal-impl.h"

#define REGISTER_TYPE_FOR_SERIALIZATION(...)                             \
    static_assert(std::is_same<__VA_ARGS__::Meta::base, void>::value,    \
                  "This macro is intended for non-derived types only."); \
    DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS(__VA_ARGS__)

#define REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(...)                    \
    static_assert(!std::is_same<__VA_ARGS__::Meta::base, void>::value,  \
                  "This macro is intended for derived types only.");    \
    CEREAL_REGISTER_TYPE(__VA_ARGS__)                                   \
                                                                        \
    /* Adapted from cereal library. See types/polymorphic.hpp there. */ \
    namespace cereal                                                    \
    {                                                                   \
    namespace detail                                                    \
    {                                                                   \
    template <>                                                         \
    struct PolymorphicRelation<__VA_ARGS__::Meta::base, __VA_ARGS__> {  \
        static void bind()                                              \
        {                                                               \
            RegisterPolymorphicCaster<__VA_ARGS__::Meta::base,          \
                                      __VA_ARGS__>::bind();             \
        }                                                               \
    };                                                                  \
    }                                                                   \
    } /* end namespaces */

// TODO why not needed for derived types?
// TODO move to impl file
#define DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS(...)                               \
    namespace cereal                                                         \
    {                                                                        \
    template <class Archive>                                                 \
    void CEREAL_SAVE_FUNCTION_NAME(Archive& archive, __VA_ARGS__ const& obj) \
    {                                                                        \
        reflect_lib::detail::save_impl(                                      \
            archive, obj, __VA_ARGS__::Meta::fields(),                       \
            std::is_same<typename __VA_ARGS__::Meta::base, void>{});         \
    }                                                                        \
                                                                             \
    template <class Archive>                                                 \
    void CEREAL_LOAD_FUNCTION_NAME(Archive& archive, __VA_ARGS__& obj)       \
    {                                                                        \
        reflect_lib::detail::load_impl(                                      \
            archive, obj, __VA_ARGS__::Meta::fields(),                       \
            std::is_same<typename __VA_ARGS__::Meta::base, void>{});         \
    }                                                                        \
                                                                             \
    }  // namespace cereal
