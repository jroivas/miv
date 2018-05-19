#include "tools.hh"
#include "utf8.h"
#include "terminal.hh"
#include <fstream>

std::string editor::substrSafe(std::string s, std::string::size_type p, std::string::size_type cnt)
{
    if (p >= utf8_length(s)) return "";

    uint32_t strlen = utf8_length(s);
    auto startpos = s.begin();

    if (strlen < p) return "";
    utf8::advance(startpos, p, s.end());

    auto endpos = startpos;
    if (p >= strlen) return "";
    if (cnt >= strlen) cnt = strlen;
    if (p + cnt > strlen) cnt = strlen - p;
    utf8::advance(endpos, cnt, s.end());

    return std::string(startpos, endpos);
}

void editor::log(std::string prefix, std::string s)
{
    std::ofstream fd("miv.log", std::ofstream::out | std::ofstream::app);
    fd << prefix << " " << s << "\n";
    fd.close();
}

bool editor::utf8_valid(std::string s)
{
    return s.end() == utf8::find_invalid(s.begin(), s.end());
}

uint32_t editor::utf8_length(std::string s)
{
    auto res = utf8::distance(s.begin(), s.end());
    uint32_t r2 = res;
    return res;
}
