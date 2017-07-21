#include <algorithm>
#include <cassert>

#include <boost/core/demangle.hpp>

#include "remangle.h"

namespace
{
std::string::size_type get_outermost_scope_pos(std::string const& name)
{
    auto const scope_pos = name.find("::");
    if (scope_pos == name.npos)
        // no scope :: in name
        return name.npos;

    auto last_pos = name.find_first_of("&, <>()[]*");
    if (last_pos == name.npos)
        last_pos = name.length();

    // scope only in template parameters
    if (last_pos < scope_pos)
        return name.npos;

    return scope_pos;
}

}  // namespace


namespace reflect_lib
{

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

std::string demangle2(std::string mangled_name)
{
    std::string repl;

    for (auto pos = mangled_name.find('_'); pos != mangled_name.npos; pos = mangled_name.find('_', pos)) {
        if (pos == mangled_name.length() - 1) break;

        switch (mangled_name[pos+1]) {
            case '_':
                mangled_name.erase(pos, 1);
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

        mangled_name.replace(pos, 2, repl);
        ++pos;
    }
    
    return mangled_name;
}

std::string strip_namespaces(std::string name)
{
    std::string::size_type scope_pos = name.find("::");
    if (scope_pos == name.npos)
        return name;

    std::string::size_type last_pos = name.find_first_of("&, <>()[]*");
    if (last_pos == name.npos)
        last_pos = name.length();

    while (scope_pos < last_pos) {
        auto const new_pos = name.find("::", scope_pos + 2);
        if (new_pos != name.npos)
            scope_pos = new_pos;
        else
            break;
    }

    if (scope_pos == name.npos)
        return name;

    return name.substr(scope_pos + 2);
}

std::string strip_outermost_namespace(std::string name)
{
    auto const scope_pos = get_outermost_scope_pos(name);

    // not scoped
    if (scope_pos == name.npos)
        return name;

    return name.substr(scope_pos + 2);
}

bool is_scoped(const std::string& name)
{
    return get_outermost_scope_pos(name) != name.npos;
}

std::string get_namespaces(const std::string& name)
{
    auto const last_pos =  name.find_first_of("&, <>()[]*");

    auto const pos = name.rfind("::", last_pos);
    if (pos == name.npos) {
        // :: not found
        return name.substr(0, last_pos);
    }

    // :: found
    return name.substr(0, pos);
}

}  // namespace reflect_lib
