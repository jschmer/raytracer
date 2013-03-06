#include <RayTracer/Scene/Camera.h>

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

    // _w == richtung nach hinten
    // _u == richtung nach rechts
    // _v == richtung nach oben
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
    float beta  = static_cast<float>(_tanFovy * ((_halfHeight - sample.y)/_halfHeight));

    vec3 dir = alpha*_u + beta*_v - _w;

    ray.pos = _eye;
    ray.dir = normalize(dir);
}

void Camera::move(Direction dir, float angle) {
    const float moving_percentage = .4f;

    switch (dir) {
    case ROTATE_LEFT:
        {
            // _w, _u ändern, um _v (up) rotieren
            auto w = _w;
            auto u = _u;
            auto rot = glm::rotate(mat4(1.f), -angle, _v);
            _w = normalize(vec3(vec4(_w, 0) * rot));
            _u = normalize(cross(_v, _w));
        }
        break;
    case ROTATE_RIGHT:
        {
            // _w, _u ändern, um _v (up) rotieren
            auto w = _w;
            auto u = _u;
            auto rot = glm::rotate(mat4(1.f), angle, _v);
            _w = normalize(vec3(vec4(_w, 0) * rot));
            _u = normalize(cross(_v, _w));
        }
        break;
    case ROTATE_UP:
        {
            // _w, _v ändern, um _u (right) rotieren
            auto w = _w;
            auto v = _v;
            auto rot = glm::rotate(mat4(1.f), -angle, _u);
            _w = normalize(vec3(vec4(_w, 0) * rot));
            _v = normalize(cross(_w, _u));
        }
        break;
    case ROTATE_DOWN:
        {
            // _w, _v ändern, um _u (right) rotieren
            auto w = _w;
            auto v = _v;
            auto rot = glm::rotate(mat4(1.f), angle, _u);
            _w = normalize(vec3(vec4(_w, 0) * rot));
            _v = normalize(cross(_w, _u));
        }
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

void Camera::moveAroundCrystalBall(Direction dir, float angle) {
    const float moving_percentage = .4f;

    // _w == richtung nach hinten
    // _u == richtung nach rechts
    // _v == richtung nach oben

    switch (dir) {
    case ROTATE_LEFT:
        {
            auto eye = _eye;
            auto w = _w;
            auto u = _u;
            auto v = _v;

            // _eye, _w, _u ändern, um _v rotieren
            auto rotation    = glm::rotate(mat4(1.f), angle, _v);

            _eye = vec3(vec4(_eye, 0) * rotation);
            _w = normalize(_eye - _center); 
            _u = normalize(vec3(vec4(_u, 0) * rotation));
            _v = cross(_w, _u);
        }
        break;
    case ROTATE_RIGHT:
        {
            // _eye, _w, _u ändern, um _v rotieren
            auto rotation    = glm::rotate(mat4(1.f), -angle, _v);

            _eye = vec3(vec4(_eye, 0) * rotation);
            _w = normalize(_eye - _center); 
            _u = normalize(vec3(vec4(_u, 0) * rotation));
            _v = cross(_w, _u);
        }
        break;
    case ROTATE_UP:
        {
            // _eye, _w, _u ändern, um _u rotieren
            auto rotation    = glm::rotate(mat4(1.f), angle, _u);

            _eye = vec3(vec4(_eye, 0) * rotation);
            _w = normalize(_eye - _center); 
            _u = normalize(vec3(vec4(_u, 0) * rotation));
            _v = cross(_w, _u);
        }
        break;
    case ROTATE_DOWN:
        {
            // _eye, _w, _u ändern, um _u rotieren
            auto rotation    = glm::rotate(mat4(1.f), -angle, _u);

            _eye = vec3(vec4(_eye, 0) * rotation);
            _w = normalize(_eye - _center); 
            _u = normalize(vec3(vec4(_u, 0) * rotation));
            _v = cross(_w, _u);
        }
        break;
    case CLOSER:
        _eye = _eye + moving_percentage*(-_w);
        break;
    case FARTHER:
        _eye = _eye + moving_percentage*_w;
        break;
    default:
        assert(false);
    }
}