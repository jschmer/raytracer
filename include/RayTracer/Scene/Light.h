#pragma once

#include <vector>

#include <RayTracer/glm_includes.h>
#include <RayTracer/Ray.h>

typedef std::vector<Ray> ShadowRays;

class Light {
public:
    Light(color3 intensity, vec3 attenuation)
        : _intensity(intensity),
        _attenuation(attenuation)
    {}

    virtual ShadowRays getShadowRaysFrom(vec3 const &point) const = 0;

    virtual float getDistanceTo(vec3 const &point) const = 0;
    virtual vec3 getIntensityAt(vec3 const &point) const = 0;

    // vars
protected:
    vec3   _attenuation;   // const, linear, quadratic term
    color3 _intensity;
};

class PointLight : public Light {
public:
    PointLight(vec3 pos, color3 intensity, vec3 attenuation);

    ShadowRays getShadowRaysFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _pos;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(vec3 dir, color3 intensity, vec3 attenuation);

    ShadowRays getShadowRaysFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _dir;
};

// provides and area light in the form of a square
class AreaLight : public Light {
public:
    // size is the edge length
    AreaLight(vec3 pos, vec3 dir, float size, color3 intensity, vec3 attenuation);

    ShadowRays getShadowRaysFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _pos;
    vec3 _dir;
    float _size;

    vec3 plane_dir_first;
    vec3 plane_dir_second;
};