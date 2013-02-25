#include "stdafx.h"
#include "CppUnitTest.h"

#include <RayTracer/Scene/Primitives/AABB.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AABBtest
{		
	TEST_CLASS(AABB_Test)
	{
	public:
		
		TEST_METHOD(AABB_Ray_Intersection)
		{
			AABB a(vec3(0, 0, 0), vec3(1, 1, 1));
            Assert::AreEqual(6, 6);
		}

        TEST_METHOD(Failing_Test)
		{
            Assert::AreEqual(2, 6);
		}

	};
}