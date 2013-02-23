#include <RayTracer/Scene/Light.h>

// glm functions
using glm::normalize;

/*
 * Point light
 */
PointLight::PointLight(vec3 pos, color3 intensity, vec3 attenuation)
    : Light(intensity, attenuation),
    _pos(pos)
{
}

Ray PointLight::getShadowRayFrom(vec3 const &point) const {
    auto dir = normalize(_pos - point);
    auto pos = point + shadow_ray_origin_offset * dir;

    return Ray(pos, dir);
}

ShadowRays PointLight::getShadowRaysFrom(vec3 const &point) const {
    return ShadowRays();
}

float PointLight::getDistanceTo(vec3 const &point) const {
    return glm::distance(_pos, point);
}

vec3 PointLight::getIntensityAt(vec3 const &point) const {
    float d = glm::distance(point, _pos);
    return _intensity / (_attenuation[0] + _attenuation[1] * d + _attenuation[2] * d * d);
}
                

/*
 * Directional light
 */
DirectionalLight::DirectionalLight(vec3 dir, color3 intensity, vec3 attenuation)
    : Light(intensity, attenuation),
    _dir(normalize(dir))
{
}

Ray DirectionalLight::getShadowRayFrom(vec3 const &point) const {
    auto pos = point + shadow_ray_origin_offset * _dir;

    return Ray(pos, _dir);
}

ShadowRays DirectionalLight::getShadowRaysFrom(vec3 const &point) const {
    return ShadowRays();
}

float DirectionalLight::getDistanceTo(vec3 const &point) const {
    return FLT_MAX;
}

vec3 DirectionalLight::getIntensityAt(vec3 const &point) const {
    return _intensity;
}