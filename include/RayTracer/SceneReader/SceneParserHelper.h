/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <sstream>
#include <vector>

bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values);

std::string getDirectory(std::string path);