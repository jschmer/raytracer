#pragma once

#include <glm/glm.hpp>
using namespace glm;

class Light {
public:
    Light(vec4 pos_or_dir, vec3 color, vec3 attenuation, mat4 transform)
        : pos_or_dir(pos_or_dir),
        color(color),
        attenuation(attenuation),
        transform(transform)
    {
        normalized_dir = normalize(vec3(pos_or_dir));
    }

    vec3 LightVectorFrom(vec3 const &point) const {
        if (pos_or_dir[3] == 1)
            // point light
            return normalize(vec3(pos_or_dir) - point);
        else
            // directional light, pos_or_dir is the light vector TO the light source
            return this->normalized_dir;
    }

    vec3 attenuation;   // const, linear, quadratic term

    vec4 pos_or_dir;
    vec3 normalized_dir;
    vec3 color;

    mat3 transform;
};