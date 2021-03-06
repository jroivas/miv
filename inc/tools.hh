#pragma once

#include <string>
#include <algorithm>

namespace editor {

std::string substrSafe(std::string s, std::string::size_type p, std::string::size_type cnt = std::string::npos);
bool utf8_valid(std::string s);
char utf8_at(std::string s, uint32_t p);
std::string utf8_at_str(std::string s, uint32_t p);
uint32_t utf8_length(std::string s);

void log(std::string prefix, std::string s);

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
}
