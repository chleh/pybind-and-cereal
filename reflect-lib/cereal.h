/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

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
    } /* end namespaces */                                              \
    DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS(__VA_ARGS__)
