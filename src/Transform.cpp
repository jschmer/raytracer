// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include <RayTracer\Transform.h>

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
    float rad = degrees*pi/180;
    vec3 ax = glm::normalize(axis);
    mat3 x(ax.x*ax, ax.y*ax, ax.z*ax);
    mat3 i(1);
    mat3 rot = i*cos(rad)+x*glm::transpose(x)*(1-cos(rad))+sin(rad)*mat3(0,ax.z,-ax.y,   -ax.z,0,ax.x,   ax.y,-ax.x,0);

    return rot;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret;
    ret[0] = vec4(sx, 0, 0, 0);
    ret[1] = vec4(0, sy, 0, 0);
    ret[2] = vec4(0, 0, sz, 0);
    ret[3] = vec4(0, 0, 0, 1);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret(1.0);
    ret[3] = vec4(tx, ty, tz, 1);
    return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
