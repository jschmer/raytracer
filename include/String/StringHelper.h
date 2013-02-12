#pragma once

#include <string>
#include <vector>

namespace String {
    std::vector<std::string> &split(const std::string &s, const char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, const char delim);

    std::string replace(std::string s, const std::string find, const std::string replace);
    std::string replaceExtension(std::string inp, const std::string new_ext);
}