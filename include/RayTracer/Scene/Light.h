#pragma once

#include <glm/glm.hpp>

// glm types
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

class Light {
public:
    Light(vec4 pos_or_dir, vec3 color, vec3 attenuation, mat4 transform);

    vec3 LightVectorFrom(vec3 const &point) const;

    // vars
    vec3 attenuation;   // const, linear, quadratic term
    vec4 pos_or_dir;
    vec3 normalized_dir;
    vec3 color;

    mat3 transform;
};