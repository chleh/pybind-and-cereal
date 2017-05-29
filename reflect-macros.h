#pragma once

#include <tuple>
#include <utility>

#include "util-macros.h"

template <typename T>
struct Type {
    using type = T;
};


#define APPLY(MACRO, ...) \
    MACRO(__VA_ARGS__)

#define IGNORE_HEAD(HEAD, ...) \
    __VA_ARGS__

#define NOOP(...)
// for debugging:
// #define NOOP0(...) "no_op0"
// #define NOOP1(...) "no_op1"
// #define NOOP2(...) "no_op2"
// #define NOOP3(...) "no_op3"
// #define NOOP4(...) "no_op4"
// #define NOOP5(...) "no_op5"
// #define NOOP6(...) "no_op6"


#define DUMMY() NOOP,  NOOP,  NOOP,  NOOP,  NOOP,  NOOP
// for debugging:
// #define DUMMY() NOOP1, NOOP2, NOOP3, NOOP4, NOOP5, NOOP6


#define GET_NAMES_POINTERS_TO_MEMBERS(CLASS, MEMBERS) \
    GET_NAMES_POINTERS_TO_MEMBERS_IMPL(CLASS, EXPAND(MEMBERS))

// works for at most five member (I assume)
// works around empty member lists
#define GET_NAMES_POINTERS_TO_MEMBERS_IMPL(CLASS, ...) \
    APPLY(GET_MACRO, DUMMY __VA_ARGS__ (), GNP2M, GNP2M, GNP2M, GNP2M, GNP2M)(CLASS, __VA_ARGS__)

// #define GET_NAMES_POINTERS_TO_MEMBERS_IMPL2(CLASS, ...)
#define GNP2M(CLASS, ...) \
    APPLY(IGNORE_HEAD, 0 FOR_EACH(MAKE_PAIR_NAME_POINTER_TO_MEMBER, CLASS, __VA_ARGS__))

#define MAKE_PAIR_NAME_POINTER_TO_MEMBER(CLASS, MEMBER) \
    , std::make_pair(#MEMBER, &CLASS::MEMBER)

#define FIELDS_TAG 42
#define METHODS_TAG 57

#define FIELDS(...) \
    FIELDS_TAG, ( __VA_ARGS__ )

#define METHODS(...) \
    METHODS_TAG, (__VA_ARGS__ )

#define REFLECT_IMPL(CLASS, FIELDS_TAG_, FIELDS, METHODS_TAG_, METHODS) \
    REFLECT_DERIVED_IMPL(CLASS, void, FIELDS_TAG_, FIELDS, METHODS_TAG_, METHODS) \

#define REFLECT(...) \
    REFLECT_IMPL(__VA_ARGS__)

#define REFLECT_DERIVED(...) \
    REFLECT_DERIVED_IMPL(__VA_ARGS__)

#define REFLECT_DERIVED_IMPL(CLASS, BASE, FIELDS_TAG_, FIELDS, METHODS_TAG_, METHODS) \
    static_assert(FIELDS_TAG_ == FIELDS_TAG, "Error wrong fields tag"); \
    static_assert(METHODS_TAG_ == METHODS_TAG, "Error wrong methods tag"); \
    struct Meta { \
        static constexpr decltype(auto) base() { return Type<BASE>{}; } \
        static constexpr decltype(auto) name() { return #CLASS; } \
        static constexpr decltype(auto) fields() \
        { \
            return std::make_tuple(GET_NAMES_POINTERS_TO_MEMBERS(CLASS, FIELDS)); \
        } \
        static constexpr decltype(auto) methods() \
        { \
            return std::make_tuple(GET_NAMES_POINTERS_TO_MEMBERS(CLASS, METHODS)); \
        } \
    };



