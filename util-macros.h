#pragma once

// From https://stackoverflow.com/a/11994395
// Make a FOREACH macro
#define FE_1(WHAT, X, Y) WHAT(X, Y) 
#define FE_2(WHAT, X, Y, ...) WHAT(X, Y)FE_1(WHAT, X, __VA_ARGS__)
#define FE_3(WHAT, X, Y, ...) WHAT(X, Y)FE_2(WHAT, X, __VA_ARGS__)
#define FE_4(WHAT, X, Y, ...) WHAT(X, Y)FE_3(WHAT, X, __VA_ARGS__)
#define FE_5(WHAT, X, Y, ...) WHAT(X, Y)FE_4(WHAT, X, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME 
#define FOR_EACH(action, X, ...) \
  GET_MACRO(__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1)(action, X, __VA_ARGS__)



#define EXPAND(SEQ) EXPAND_IMPL SEQ
#define EXPAND_IMPL(...) __VA_ARGS__



