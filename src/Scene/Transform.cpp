#include <RayTracer/Scene/Transform.h>

const auto PI = 3.14159265358979323846f;

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
    float rad = degrees*PI/180;

    vec3 ax = glm::normalize(axis);
    mat3 x(axis, axis, axis);
    mat3 i(1);
    mat3 rot = i*cos(rad)+x*glm::transpose(x)*(1-cos(rad))+sin(rad)*mat3(0,axis.z,-axis.y,   -axis.z,0,axis.x,   axis.y,-axis.x,0);

    return rot;
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(const float degrees, vec3& eye, vec3& axis) {
    mat3 rot = Transform::rotate(degrees, axis);

    eye = rot*eye;
    // up vector is the same?
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(const float degrees, vec3& eye, vec3& up, vec3& axis) {
    mat3 rot = Transform::rotate(degrees, axis);

    eye = rot*eye;
    up  = rot*up;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
