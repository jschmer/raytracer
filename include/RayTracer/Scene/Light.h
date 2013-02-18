#pragma once

#include <RayTracer/glm_includes.h>

class Light {
public:
    Light(vec4 pos_or_dir, color3 color, vec3 attenuation, mat4 transform);

    vec3 LightVectorFrom(vec3 const &point) const;

    // vars
    vec3 attenuation;   // const, linear, quadratic term
    vec4 pos_or_dir;
    vec3 normalized_dir;
    color3 color;

    mat3 transform;
};