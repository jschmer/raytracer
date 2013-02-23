#pragma once

#include <vector>

#include <RayTracer/glm_includes.h>
#include <RayTracer/Ray.h>

typedef std::vector<Ray> ShadowRays;

const float shadow_ray_origin_offset = .0001f;

class Light {
public:
    Light(color3 intensity, vec3 attenuation)
        : _intensity(intensity),
        _attenuation(attenuation)
    {}

    virtual Ray getShadowRayFrom(vec3 const &point) const = 0;
    virtual ShadowRays getShadowRaysFrom(vec3 const &point) const = 0;

    virtual float getDistanceTo(vec3 const &point) const = 0;
    virtual vec3 getIntensityAt(vec3 const &point) const = 0;

    // vars
    vec3   _attenuation;   // const, linear, quadratic term
    color3 _intensity;
};

class PointLight : public Light {
public:
    PointLight(vec3 pos, color3 intensity, vec3 attenuation);

    Ray getShadowRayFrom(vec3 const &point) const override;
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

    Ray getShadowRayFrom(vec3 const &point) const override;
    ShadowRays getShadowRaysFrom(vec3 const &point) const override;

    float getDistanceTo(vec3 const &point) const override;
    vec3 getIntensityAt(vec3 const &point) const override;

private:
    // vars
    vec3 _dir;
};

//class AreaLight {
//public:
//    AreaLight(vec3 pos, vec3 dir, color3 intensity, vec3 attenuation);
//
    //vec3 getLightVectorFrom(vec3 const &point) const override;
    //LightRayDirections getLightDirectionsFrom(vec3 const &point) const override;

    //float getDistanceTo(vec3 const &point) const override;
    //vec3 getIntensityAt(vec3 const &point) const override;
//
//    // vars
//    vec3 _pos;
//    vec3 _dir;
//};