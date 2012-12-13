#pragma once

#ifndef HELPER_H
#define HELPER_H

#include <glm\glm.hpp>
#include <iostream>
#include <iomanip>
#include <string>

using namespace glm;
using namespace std;

float vecLen(vec3 &vec);
void printMat4(mat4 &mat, std::string title);
void printVec4(vec4 &vec, std::string title);

#endif