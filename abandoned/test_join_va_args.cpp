/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */


// https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

#define APPLY(MACRO, ...) MACRO(__VA_ARGS__)

#define CONCAT3(a, b, c) CONCAT3_(a, b, c)
#define CONCAT3_(a, b, c) a, XX, b, XX, c

#define JOIN(sep, head, next, ...) \
    JOIN_(sep, head, next, __VA_ARGS__)
#define JOIN_(sep, head, next, ...) \
    CONCAT3(head, sep, EXPAND(DEFER(next)(sep, __VA_ARGS__)))
// #define JOIN(sep, head, next, ...) \
//     APPLY(CONCAT3, head, sep, next(__, __VA_ARGS__))

#define STOP(...)


JOIN(__, 1, STOP) // 1__
JOIN(__, 1, JOIN, 2, STOP) // 1__
JOIN(__, 1, JOIN, 2, JOIN, 3, STOP)
// --> 1__2__3
JOIN(__, 1, JOIN(__, 2, JOIN, 3, STOP))
JOIN(__, 1, JOIN(__, 2, JOIN(__, 3, STOP)))


