#include <RayTracer/Scene/Primitives/Triangle.h>

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Material.h>

// glm functions
using glm::transpose;
using glm::normalize;
using glm::dot;
using glm::cross;


Triangle::Triangle(mat4 obj2world, vec3 &f, vec3 &g, vec3 &h)
    : Primitive(obj2world)
{
    v0 = f;
    v1 = g;
    v2 = h;

    vec4_faceNormal = vec4(glm::cross(v1 - v0, v2 - v0), 0); 
    vec3_faceNormal = vec3(vec4_faceNormal);
    transformedNormal = normalize(vec3(transpose(this->world2obj) * vec4_faceNormal));
    hasTextureCoords = false;

    // cached triangle data (for intersect method)
    v0v1 = v1 - v0;    // v0 nach v1!
    v0v2 = v2 - v0;
    v1v2 = v2 - v1;
    v2v0 = v0 - v2;

    normal_len = dot(vec3_faceNormal, vec3_faceNormal);
    d = dot(vec3_faceNormal, v0);
}

// taken from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/barycentric-coordinates/
float Triangle::Intersect(Ray const &ray, Intersection &Hit, float dist) {
    // transforming ray to object space
    Ray r;
    r.pos = vec3(this->world2obj * vec4(ray.pos, 1));
    r.dir = vec3(this->world2obj * vec4(ray.dir, 0));

    float nDotRay = dot(vec3_faceNormal, r.dir);

    if (nDotRay == 0)
        return -1.0f; // ray parallel to triangle

    float t = -(dot(vec3_faceNormal, r.pos) - d) / nDotRay;

    // only consider objects closer than the closest intersection until now
    if (t > dist)
        return -1.0f;

    // compute intersection point
    vec3 Phit = r.pos + t * r.dir;

    vec3 v0p = Phit - v0;

    //float bu, bv, bw;
    //Barycentric(Phit, bu, bv, bw);

    // inside-out test edge0
    float v = dot(vec3_faceNormal, cross(v0v1, v0p));
    if (v < 0)
        return -1.0f; // P outside triangle

    // inside-out test edge1
    float w = dot(vec3_faceNormal, cross(v1v2, Phit - v1));
    if (w < 0)
        return -1.0f; // P outside triangle

    // inside-out test edge2
    float u = dot(vec3_faceNormal, cross(v2v0, Phit - v2));
    if (u < 0)
        return -1.0f; // P outside triangle

    // calculate barycentric coords
    u = u / normal_len;
    v = v / normal_len;
    w = 1 - u - v;

    // interpolate texture coords
    // p = w
    // p - p0 = (p2 - p1, p1 - p0)()
    // solve: 

    // storing intersection params
    Hit.has_hit  = true;
    Hit.t        = t;
    Hit.hitPoint = vec3(this->obj2world * vec4(Phit, 1));

    // TODO: normal interpolation at hitpoint with barycentric coordinates (u, v, w)
    Hit.normal   = this->transformedNormal;

    // auto hitpoint_vec = w*v0 + u*v1 + v*v2;
    // w gehört zu v0
    // u gehört zu v1
    // v gehört zu v2

    // storing material color
    // fetch diffuse color from texture!
    if (HasTextureCoords()) {
        if (mat->HasTexture(TextureType::AMBIENT)) {
            auto interpolated_texture_coord = w*vt0 + u*vt1 + v*vt2;
            Hit.material_color.ambient = mat->getTextureColor(TextureType::AMBIENT, interpolated_texture_coord[0], interpolated_texture_coord[1]);
        }
        else {
            Hit.material_color.ambient = mat->ambient;
        }

        if (mat->HasTexture(TextureType::DIFFUSE)) {
            auto interpolated_texture_coord = w*vt0 + u*vt1 + v*vt2;
            Hit.material_color.diffuse = mat->getTextureColor(TextureType::DIFFUSE, interpolated_texture_coord[0], interpolated_texture_coord[1]);
        }
        else {
            Hit.material_color.diffuse = mat->diffuse;
        }

        if (mat->HasTexture(TextureType::SPECULAR)) {
            auto interpolated_texture_coord = w*vt0 + u*vt1 + v*vt2;
            Hit.material_color.specular = mat->getTextureColor(TextureType::SPECULAR, interpolated_texture_coord[0], interpolated_texture_coord[1]);
        }
        else {
            Hit.material_color.specular = mat->specular;
        }
    } else {
        Hit.material_color.ambient  = mat->ambient;
        Hit.material_color.diffuse  = mat->diffuse;
        Hit.material_color.specular = mat->specular;
    }
    
    Hit.material_color.emission  = mat->emission;
    Hit.material_color.shininess = mat->shininess;

    return t;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Triangle::Barycentric(vec3 phit, float &u, float &v, float &w)
{
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 v0p  = phit - v0;
    float d00 = dot(v0v1, v0v1);
    float d01 = dot(v0v1, v0v2);
    float d11 = dot(v0v2, v0v2);
    float d20 = dot(v0p, v0v1);
    float d21 = dot(v0p, v0v2);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

bool Triangle::HasTextureCoords() {
    return hasTextureCoords;
}