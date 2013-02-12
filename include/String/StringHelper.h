#pragma once

#include <string>
#include <vector>

namespace String {
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);

    void replace(std::string &s, const std::string find, const std::string replace);
}