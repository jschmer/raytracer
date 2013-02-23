#pragma once

#include <vector>

#include <RayTracer/glm_includes.h>

typedef std::vector<vec3> LightRayDirections;

class Light {
public:
    Light(color3 intensity, vec3 attenuation)
        : _intensity(intensity),
        _attenuation(attenuation)
    {}

    virtual vec3 getLightVectorFrom(vec3 const &point) const = 0;
    virtual LightRayDirections getLightDirectionsFrom(vec3 const &point) const = 0;

    virtual float getDistanceTo(vec3 const &point) const = 0;
    virtual vec3 getIntensityAt(vec3 const &point) const = 0;

    // vars
    vec3   _attenuation;   // const, linear, quadratic term
    color3 _intensity;
};

class PointLight : public Light {
public:
    PointLight(vec3 pos, color3 intensity, vec3 attenuation);

    vec3 getLightVectorFrom(vec3 const &point) const override;
    LightRayDirections getLightDirectionsFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _pos;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(vec3 dir, color3 intensity, vec3 attenuation);

    vec3 getLightVectorFrom(vec3 const &point) const override;
    LightRayDirections getLightDirectionsFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _dir;
};

//class AreaLight {
//public:
//    AreaLight(vec4 pos_or_dir, color3 intensity, vec3 attenuation);
//
//    vec3 LightVectorFrom(vec3 const &point) const;
//
//    // vars
//    vec3 v1, v2, v3, v4;
//
//    vec3 attenuation;   // const, linear, quadratic term
//    color3 intensity;
//
//    mat3 transform;
//};