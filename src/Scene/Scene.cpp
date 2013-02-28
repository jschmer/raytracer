#include <RayTracer/Scene/Scene.h>

#include <iostream>
#include <future>

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Light.h>
#include <RayTracer/Scene/Primitives/Primitive.h>
#include <RayTracer/RenderTarget/PNGImage.h>
#include <RayTracer/Scene/Transform.h>

Scene::Scene()
    : _hasSize(false),  // default values
    _maxdepth(5),
    _camera(nullptr)
{
    _size.height = 600;
    _size.width  = 600;
}

Scene::~Scene() {
    for (auto prim : _primitives)
        delete prim;

    for (auto light : _lights)
        delete light;

    for (auto mat : _materials)
        delete mat;
}

void Scene::createAABB() {
    float minx, miny, minz;
    minx = miny = minz = FLT_MAX;

    float maxx, maxy, maxz;
    maxx = maxy = maxz = FLT_MIN;

    for (auto prim : _primitives) {
        // get the primitives bounding box
        auto box = prim->getAABB();

        // min values for scene AABB
        minx = common::min(minx, box._min.x);
        miny = common::min(miny, box._min.y);
        minz = common::min(minz, box._min.z);

        // max values for scene AABB
        maxx = common::max(maxx, box._max.x);
        maxy = common::max(maxy, box._max.y);
        maxz = common::max(maxz, box._max.z);

        // store adress of primitives in overlapping boxes
    }

    _scene_aabb._min = vec3(minx, miny, minz);
    _scene_aabb._max = vec3(maxx, maxy, maxz);

    auto tenth = _primitives.size() / 10;
    unsigned int aabb_dimension = (tenth != 0) ? tenth : 1;
    aabb_dimension = static_cast<unsigned int>(ceil(pow( aabb_dimension, 1./3.)));
    aabb_dimension = common::min(aabb_dimension, 10u);      // cap at 1000 AABBs
        
    std::cout << " Using " << aabb_dimension*aabb_dimension*aabb_dimension << " AABBs" << std::endl;

    // create aabb_dimension*aabb_dimension*aabb_dimension boxes in the AABB of the scene
    float dsizeX = ((maxx - minx) / aabb_dimension) * 1.0001f; // The mul with 1.0001f accounts for a little imprecision, it makes the boxes a bit bigger than the scene AABB
    float dsizeY = ((maxy - miny) / aabb_dimension) * 1.0001f;
    float dsizeZ = ((maxz - minz) / aabb_dimension) * 1.0001f;

    _aabbs.clear();
    for (float dminx = minx, dmaxx = minx + dsizeX; dminx <= maxx; ) {
        for (float dminy = miny, dmaxy = miny + dsizeY; dminy <= maxy; ) {
            for (float dminz = minz, dmaxz = minz + dsizeZ; dminz <= maxz; ) {
                // do the work
                _aabbs.push_back(AABB(vec3(dminx, dminy, dminz),
                                      vec3(dmaxx, dmaxy, dmaxz)));

                // after-run work
                dminz = dmaxz;
                dmaxz += dsizeZ;
            }

            // after-run work
            dminy = dmaxy;
            dmaxy += dsizeY;
        }

        // after-run work
        dminx  = dmaxx;
        dmaxx += dsizeX;
    }

    // fill the aabbs
    for (auto& aabb : _aabbs) {
        aabb._primitives.clear();
        for (auto prim : _primitives) {
            if (aabb.Intersect(prim->getAABB()))
                aabb._primitives.push_back(prim);
        }
    }
}

void Scene::createDefaultCamera() {
    std::cout << " >> No Camera detected! Setting one on the Z-Axis..." << std::endl;

    // create a camera that looks at the center of the scene-bounding-box
    // and that shows the entire scene
    vec3 eye, lookAt, up(0, 1, 0);

    // calculating the center of the scenes bounding box
    auto& max = _scene_aabb._max;
    auto& min = _scene_aabb._min;

    lookAt.x = (max.x + min.x) / 2.0f;
    lookAt.y = (max.y + min.y) / 2.0f;
    lookAt.z = (max.z + min.z) / 2.0f;

    // calculating the distance of the camera to the scene
    // has to be the distance where the camera can see the complete scene
    float aspect = (float)_size.width/_size.height;
    float FOVy_degree = 45.0f;
    float FOVy = radians(FOVy_degree);
    float FOVx = FOVy*aspect;          // basis of thought: (height/width) = (FOVy/FOVx)

    auto half_AABB_height = (max.y - min.y)/2.0f;
    auto half_AABB_width  = (max.x - min.x)/2.0f;
    auto Zlen_height = half_AABB_height/tan(FOVy/2.0f);
    auto Zlen_width  = half_AABB_width /tan(FOVx/2.0f);

    // Zlen was calculated relative to z = 0
    // --> add maximum z-value of AABB
    Zlen_height += max.z;
    Zlen_width  += max.z;

    // offset the distance a bit further to get a bit space from the scene to the image border
    Zlen_height *= 1.1f;
    Zlen_width  *= 1.1f;

    eye.x = lookAt.x;
    eye.y = lookAt.y;
    eye.z = common::max(Zlen_height, Zlen_width);

    _camera.reset(new Camera(eye, lookAt, up, FOVy_degree));

    // set up a light if there isn't one
    if (_lights.size() == 0) {
        std::cout << " >> No  Light detected! Setting up directional light on the Z-Axis..." << std::endl;
        createDefaultLight();
    }
}

void Scene::createDefaultLight() {
    // create a directional light that shines down the Z-axis
    _lights.push_back(new DirectionalLight(vec3(0, 0, 1), vec3(1, 1, 1)));
}

bool Scene::inShadow(Ray const &ray, float t_hit = FLT_MAX) {
    Intersection hit;
    float t;

    // loop through the AABBs to see if the ray intersects them
    for (auto const& aabb : _aabbs) {
        // and if it does: do the primitive intersection
        if (!aabb.isEmpty() && aabb.Intersect(ray, t)) {
            for (auto const primitive : aabb._primitives) {
                t = primitive->Intersect(ray, hit);
                if (t > 0 && t < t_hit) {
                    return true;
                }
            }
        }
    }

    return false;
}

Intersection Scene::trace(Ray const &ray, int depth) {
    // first fast test: does the ray intersect the scene AABB?
    float t_hit = FLT_MAX;
    Intersection intersection;
    if (!_scene_aabb.Intersect(ray, t_hit)) {
        return intersection;
    }

    if (depth <= this->_maxdepth) {
        float t;
        Intersection hit;
        // loop through the AABBs to see if the ray intersects them
        for (auto const& aabb : _aabbs) {
            // and if it does: do the primitive intersection
            if (!aabb.isEmpty() && aabb.Intersect(ray, t)) {
                for (auto const primitive : aabb._primitives) {
                    t = primitive->Intersect(ray, hit, t_hit);
                    if (t > 0 && t < t_hit) {
                        t_hit = t;
                        intersection = hit;
                    }
                }
            }
        }
    }

    // got a hit point -> get color!
    if (intersection.has_hit) {
        intersection.color = shade(intersection, ray, depth);
    }

    return intersection;
}

vec3 Scene::shade(Intersection &Hit, Ray const &ray, int depth) {
    vec3 out_color(0.0);

    auto& material_color = Hit.material_color;

    // setting material props
    // die infos von der Intersection benutzen!
    // die verschiedenen farben sollten in der Intersect methode der Primitiven gesetzt werden
    // dort kann auch auf die texturen zugegriffen werden
    vec3& diffuse    = material_color.diffuse;
    vec3& ambient    = material_color.ambient;
    vec3& specular   = material_color.specular;
    vec3& emission   = material_color.emission;
    float& shininess = material_color.shininess;

    out_color += ambient;
    out_color += emission;

    for (auto it = _lights.begin(); it != _lights.end(); ++it) {
        auto p_light = *it;
        auto shadow_rays = p_light->getShadowRaysFrom(Hit.hitPoint);

        // test every shadow ray sample, if the ray is not in shadow, it contributes 1/number of the shadowrays 
        // to the color intensity
        float shadow_intensity = 1.0f/shadow_rays.size();
        for (auto& shadow_ray : shadow_rays) {
            // light in front of object?
            float dotP = dot(Hit.normal, shadow_ray.dir);
            if (dot(Hit.normal, shadow_ray.dir) > 0) {    
                // distance from hitpoint to light
                float max_dist = p_light->getDistanceTo(shadow_ray.pos);

                // Hitpoint ist im Schatten wenn inShadow die erste Intersection zwischen Hitpoint und Light position findet
                if (inShadow(shadow_ray, max_dist))
                    continue;       // pixel in shadow don't contribute to output color

                vec3 light_intensity = p_light->getIntensityAt(Hit.hitPoint);

                // diffuse term
                out_color += shadow_intensity * light_intensity * (diffuse * common::max(dotP, 0.0f));

                // specular term
                vec3 halfVec    = normalize(shadow_ray.dir + -ray.dir);
                float halfAngle = dot(Hit.normal, halfVec);

                out_color += shadow_intensity * light_intensity * (specular * pow(common::max(halfAngle, 0.0f), shininess));
            }
        }
 }

    if (color3(0.0f) != specular) {
        // reflection
        // R = V – 2 * (V·N) * N 
        Ray reflectionRay;
        reflectionRay.dir = ray.dir - 2 * dot(ray.dir, Hit.normal) * Hit.normal;
        reflectionRay.pos = Hit.hitPoint + 0.01f*reflectionRay.dir;
    
        Intersection t = this->trace(reflectionRay, depth+1);
        if (t.has_hit)
            out_color += specular * t.color;
    }

    // TODO: Refraction

    return out_color;
}

void Scene::moveCamera(Direction dir, float amount_degrees) {
    auto& eye = _camera->eye;
    auto& up  = _camera->up;
    auto& center  = _camera->center;

    const float moving_percentage = .5f;
    vec3 looking_dir = glm::normalize(center - eye);

    // TODO
    /*
        1. Translate so that rotation axis passes through origin
        2. Rotate so that the rotation axis is aligned with one of the principle coordinate axes
        3. Perform rotation of object about coordinate axis
        4. Perform inverse rotation of step 2
        5. Perform iInverse translation of step 1
    */

    switch (dir) {
    case LEFT: 
        //Transform::left(amount_degrees, eye, up);
        break;
    case RIGHT:
        //Transform::left(-amount_degrees, eye, up);
        break;
    case UP:
        //Transform::up(amount_degrees, eye, up, up);
        break;
    case DOWN:
        //Transform::up(-amount_degrees, eye, up, up);
        break;
    case CLOSER:
        eye = eye + moving_percentage*looking_dir;
        break;
    case FARTHER:
        eye = eye + moving_percentage*-looking_dir;
        break;
    default:
        assert(false);
    }
}