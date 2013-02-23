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

vec3 PointLight::getLightVectorFrom(vec3 const &point) const {
    return normalize(_pos - point);
}

LightRayDirections PointLight::getLightDirectionsFrom(vec3 const &point) const {
    return LightRayDirections();
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

vec3 DirectionalLight::getLightVectorFrom(vec3 const &point) const {
    return _dir;
}

LightRayDirections DirectionalLight::getLightDirectionsFrom(vec3 const &point) const {
    return LightRayDirections();
}

float DirectionalLight::getDistanceTo(vec3 const &point) const {
    return FLT_MAX;
}

vec3 DirectionalLight::getIntensityAt(vec3 const &point) const {
    return _intensity;
}