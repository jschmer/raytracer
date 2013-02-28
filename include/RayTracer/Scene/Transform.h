#include <RayTracer/glm_includes.h>

class Transform  
{
public:
	Transform();
	virtual ~Transform();

	static void left(const float degrees, vec3& eye, vec3& up);
	static void up  (const float degrees, vec3& eye, vec3& up, vec3& axis);

private:
	static mat3 rotate(const float degrees, const vec3& axis);
};

