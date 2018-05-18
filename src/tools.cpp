#include "tools.hh"

std::string editor::substrSafe(std::string s, std::string::size_type p, std::string::size_type cnt)
{
    if (p >= s.length()) return "";
    return s.substr(p, cnt);
}
