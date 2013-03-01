#include <RayTracer/Scene/Camera.h>

#include <RayTracer/Scene/Transform.h>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale


Camera::Camera(vec3 eye, vec3 center, vec3 up, float fovy)
    : _eye(eye),
    _center(center),
    _up(up),
    _fovx(0.0),
    _fovy(fovy)
{
    _looking_dir = glm::normalize(center - eye);

    // origin = eye
    // v - w = vec, der von punkt w auf punkt v zeigt
    // -> eye - center = vector, der von center auf eye zeigt
    _w = normalize(_eye - _center);
    _u = normalize(cross(_up, _w));
    _v = cross(_w, _u);
}

void Camera::initFov(float width, float height) {
    double aspect = width/height;
    _fovy    = radians(_fovy);
    _tanFovx = tan(_fovy/2.0f)*aspect;
    _tanFovy = tan(_fovy/2.0f);

    _halfHeight = height/2.0f;
    _halfWidth = width/2.0f;
}

void Camera::generateRay(Sample& sample, Ray &ray) {
    float alpha = static_cast<float>(_tanFovx * ((sample.x - _halfWidth)/_halfWidth));
    float beta = static_cast<float>(_tanFovy * ((_halfHeight - sample.y)/_halfHeight));

    vec3 dir = alpha*_u + beta*_v - _w;

    ray.pos = _eye;
    ray.dir = normalize(dir);
}

void Camera::move(Direction dir, float amount_degrees) {
    const float moving_percentage = .5f;
    
    // TODO
    /*
        1. Translate so that rotation axis passes through origin
        2. Rotate so that the rotation axis is aligned with one of the principle coordinate axes
        3. Perform rotation of object about coordinate axis
        4. Perform inverse rotation of step 2
        5. Perform iInverse translation of step 1
    */

    switch (dir) {
    case ROTATE_LEFT: 
        break;
    case ROTATE_RIGHT:
        break;
    case ROTATE_UP:
        break;
    case ROTATE_DOWN:
        break;
    case STRAFE_LEFT: 
        _eye = _eye + moving_percentage*(-_u);
        break;
    case STRAFE_RIGHT:
        _eye = _eye + moving_percentage*_u;
        break;
    case STRAFE_UP:
        _eye = _eye + moving_percentage*(_v);
        break;
    case STRAFE_DOWN:
        _eye = _eye + moving_percentage*(-_v);
        break;
    case CLOSER:
        _eye = _eye + moving_percentage*_w;
        break;
    case FARTHER:
        _eye = _eye + moving_percentage*(-_w);
        break;
    default:
        assert(false);
    }
}