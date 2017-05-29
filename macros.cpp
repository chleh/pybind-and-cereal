#include <iostream>

// #define STR(s) #s
// 
// #define MSG(prefix, ...) \
//     STR(prefix, __VA_ARGS__)


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

#define ADD_PREFIX(prefix, str) prefix #str
#define MSG(prefix, ...) \
    FOR_EACH(ADD_PREFIX, prefix, __VA_ARGS__)
#define MSG2(prefix, ...) \
    MSG(prefix, __VA_ARGS__) "--" MSG(prefix, __VA_ARGS__)



#define MSG3(SEQ1, SEQ2) \
    MSG SEQ1 "--" MSG SEQ2


#define MSG5(...) \
    MSG4(__VA_ARGS__)

#define MSG4(prefix, fields_tag, SEQ) \
    #fields_tag ": " MSG(prefix, EXPAND(SEQ))

#define FIELDS_TAG 42

#define FIELDS(...) \
    FIELDS_TAG, (__VA_ARGS__)

#define EXPAND(SEQ) EXPAND_IMPL SEQ
#define EXPAND_IMPL(...) __VA_ARGS__


#define DUMMY() \
    "dummy called!\n"

#define COMMENT(first, ...) \
    first

#define MACRO(...) \
    COMMENT(DUMMY __VA_ARGS__ ()) std::cout << "test\n"


int main()
{
    std::cout << MSG2("T", a, b, c) << '\n';
    std::cout << MSG3(("T", a, b, c), ("S", d, e, f)) << '\n';

    std::cout << MSG5("T", FIELDS(a, b, c)) << '\n';

    MACRO(1);
    MACRO(1, 2);
    MACRO(1, 2, 3);
    MACRO();
    return 0;
}
