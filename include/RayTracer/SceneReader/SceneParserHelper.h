#pragma once

#include <sstream>
#include <vector>

bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values);

std::string getDirectory(std::string path);