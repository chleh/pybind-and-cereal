#include <algorithm>
#include <cassert>

#include <boost/core/demangle.hpp>

#include "remangle.h"

std::string demangle(const char* mangled_name)
{
    // TODO does the returned string have to be normalized?
    return boost::core::demangle(mangled_name);
}

std::string mangle(std::string s)
{
    using namespace std::string_literals;

    auto replace_str1 = [](std::string& s, char const c, std::string const& repl) {
        for (auto pos = s.find(c); pos != s.npos; pos = s.find(c, pos + repl.size())) {
            s.replace(pos, 1, repl);
        }
    };

    auto replace_str2 = [](std::string& s, std::string const& search_for, std::string const& repl) {
        for (auto pos = s.find(search_for); pos != s.npos; pos = s.find(search_for, pos + repl.size())) {
            s.replace(pos, search_for.size(), repl);
        }
    };

    replace_str1(s, '_', "__");

    replace_str2(s, "&&", "_A"); // ampersand
    replace_str2(s, "::", "_S"); // scope
    replace_str2(s, ", ", "_c"); // comma
    // TODO check further cases, like *const
    replace_str2(s, " const", "_C"); // const

    auto const non_word = "&:, <>()[]*"s;

    // remove unneeded spaces
    for (auto it = std::find(s.begin(), s.end(), ' ');
            it != s.end();
            it = std::find(it, s.end(), ' '))
    {
        if (it == s.begin() || it == s.end() - 1) {
            s.erase(it);
            continue;
        }
        if (non_word.find(it[-1]) != non_word.npos
                || non_word.find(it[+1]) != non_word.npos)
        {
            // previous or next character is a non-word character
            s.erase(it);
            continue;
        }

        // TODO Can that happen?
        // space not erased
        ++it;
    }

    replace_str1(s, ' ', "_s"); // space
    replace_str1(s, ',', "_c"); // comma

    replace_str1(s, '<', "_L"); // left
    replace_str1(s, '>', "_R"); // right

    replace_str1(s, '(', "_l"); // left
    replace_str1(s, ')', "_r"); // right

    replace_str1(s, '[', "_k"); // left  l -> k
    replace_str1(s, ']', "_q"); // right r -> q

    replace_str1(s, '&', "_a"); // ampersand
    replace_str1(s, '*', "_p"); // pointer

    assert(s.find_first_of(non_word) == s.npos);
    return s;
}

std::string demangle2(std::string name)
{
    std::string repl;

    for (auto pos = name.find('_'); pos != name.npos; pos = name.find('_', pos)) {
        if (pos == name.length() - 1) break;

        switch (name[pos+1]) {
            case '_':
                name.erase(pos, 1);
                pos++;
                continue;
            case 'A':
                repl = "&&";
                break;
            case 'S':
                repl = "::";
                break;
            case 'c':
                repl = ",";
                break;
            case 'C':
                repl = " const";
                break;
            case 's':
                repl = " ";
                break;
            case 'L':
                repl = "<";
                break;
            case 'R':
                repl = ">";
                break;
            case 'l':
                repl = "(";
                break;
            case 'r':
                repl = ")";
                break;
            case 'k':
                repl = "[";
                break;
            case 'q':
                repl = "]";
                break;
            case 'a':
                repl = "&";
                break;
            case 'p':
                repl = "*";
                break;
        }

        name.replace(pos, 2, repl);
        ++pos;
    }
    
    return name;
}

