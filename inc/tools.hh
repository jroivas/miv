#pragma once

#include <string>

namespace editor {

std::string substrSafe(std::string s, std::string::size_type p, std::string::size_type cnt = std::string::npos);

}
