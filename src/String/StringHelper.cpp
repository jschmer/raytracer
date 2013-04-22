/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <String/StringHelper.h>

#include <sstream>

namespace String {
    std::vector<std::string> &split(const std::string &s, const char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> split(const std::string &s, const char delim) {
        std::vector<std::string> elems;
        return split(s, delim, elems);
    }

    std::string replace(std::string s, const std::string find, const std::string replace) {
        size_t len = find.length();
        size_t pos;
        while ((pos = s.find(find)) != std::string::npos) {
            s.replace(pos, len, replace);
        }

        return s;
    }

    std::string replaceExtension(std::string inp, std::string new_ext) {
        auto point_pos = inp.find_last_of('.');
        if (point_pos == std::string::npos)
            return inp;

        return replace(inp, inp.substr(point_pos, inp.length() - 1), "." + new_ext);
    }
}