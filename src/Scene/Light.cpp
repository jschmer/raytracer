#include <RayTracer/Scene/Light.h>

// glm functions
using glm::normalize;

const float shadow_ray_origin_offset = .001f;
const int SampleSize = 4;  // SampleSize*SampleSize shadow rays are generated

/*
 * Point light
 */
PointLight::PointLight(vec3 pos, color3 intensity, vec3 attenuation)
    : Light(intensity, attenuation),
    _pos(pos)
{
}

ShadowRays PointLight::getShadowRaysFrom(vec3 const &point) const {
    auto dir = normalize(_pos - point);
    auto pos = point + shadow_ray_origin_offset * dir;

    ShadowRays rays;
    rays.push_back(Ray(pos, dir));
    return rays;
}

float PointLight::getDistanceTo(vec3 const &point) const {
    return glm::distance(_pos, point);
}

vec3 PointLight::getIntensityAt(vec3 const &point) const {
    float d = getDistanceTo(point);
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

ShadowRays DirectionalLight::getShadowRaysFrom(vec3 const &point) const {
    auto pos = point + shadow_ray_origin_offset * _dir;

    ShadowRays rays;
    rays.push_back(Ray(pos, _dir));
    return rays;
}

float DirectionalLight::getDistanceTo(vec3 const &point) const {
    return FLT_MAX;
}

vec3 DirectionalLight::getIntensityAt(vec3 const &point) const {
    return _intensity;
}

/*
 * Area light
 */
AreaLight::AreaLight(vec3 pos, vec3 dir, float size, color3 intensity, vec3 attenuation)
    : Light(intensity, attenuation),
    _dir(normalize(-dir)),  // convert facing direction to direction TO the light
    _pos(pos),
    _size(size)
{
    // find two vectors that are vertical to _dir
    // plane_dir_first is computed via the scalar product: vec1*vec2 = 0
    // plane_dir_second is computed via cross

    // find element in _dir that is not 0
    auto not_null_idx = 0u;
    for (auto i = 0u; i < 3; ++i){
        if (_dir[i] != 0.0f) {
            not_null_idx = i;
            break;
        }
    }

    float sum = 0.0f;
    for (auto i = 0u; i < 3; ++i) {
        if (i != not_null_idx) {
            sum += _dir[i];
            _plane_dir_first[i] = 1.0f;
        }
    }

    _plane_dir_first[not_null_idx] = -sum / _dir[not_null_idx];
    _plane_dir_second = cross(_dir, _plane_dir_first);
}

ShadowRays AreaLight::getShadowRaysFrom(vec3 const &point) const {
    float stepsize = _size/SampleSize;
    float lower = -SampleSize/2.0f;
    float upper = SampleSize/2.0f;

    auto deltaX = stepsize*_plane_dir_first;
    auto deltaY = stepsize*_plane_dir_second;

    auto randMax = static_cast<float>(RAND_MAX) * 1.2f;

    // generate directions with samples over the area of the light
    // TODO: add in some noise to reduce banding
    ShadowRays rays;
    for (auto i = lower; i < upper; ++i) {
        for (auto k = lower; k < upper; ++k) {
            auto randX = rand() / randMax;
            auto randY = rand() / randMax;

            auto light_pos = _pos + (i+randX)*deltaX + (k+randY)*deltaY;
            auto dir = normalize(light_pos - point);
            auto ray_pos = point + shadow_ray_origin_offset * dir;
            rays.push_back(Ray(ray_pos, dir));
        }
    }
    return rays;
}

float AreaLight::getDistanceTo(vec3 const &point) const {
    return glm::distance(_pos, point);
}

vec3 AreaLight::getIntensityAt(vec3 const &point) const {
    return _intensity;
}