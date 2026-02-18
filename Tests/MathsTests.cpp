#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "Log.h"
#include "Maths/MathUtils.h"
#include "Maths/Vector.h"
#include "Maths/Matrix.h"

using namespace Math;

#define TEST_ASSERT(cond, msg) do { \
	if (!(cond)) { \
		std::cerr << "FAIL: " << (msg) << " (at " << __FILE__ << ":" << __LINE__ << ")\n"; \
		return 1; \
	} \
} while(0)

int RunMathsTests() {
	int failures = 0;

	// MathUtils
	TEST_ASSERT(IsFloatEqual(Deg2Rad(0.f), 0.f), "Deg2Rad(0)");
	TEST_ASSERT(IsFloatEqual(Deg2Rad(180.f), (float)M_PI), "Deg2Rad(180)");
	TEST_ASSERT(IsFloatEqual(Deg2Rad(-90.f), (float)(-M_PI/2)), "Deg2Rad(-90)");
	TEST_ASSERT(IsFloatEqual(Rad2Deg((float)M_PI), 180.f), "Rad2Deg(PI)");
	TEST_ASSERT(IsFloatEqual(Rad2Deg(0.f), 0.f), "Rad2Deg(0)");
	TEST_ASSERT(IsFloatEqual(1.f, 1.f), "IsFloatEqual same");
	TEST_ASSERT(IsFloatEqual(1.f, 1.f + EPSILON * 0.5f), "IsFloatEqual within epsilon");

	// Vec3
	Vec3 a(1, 2, 3);
	Vec3 b(4, 5, 6);
	Vec3 sum = a + b;
	TEST_ASSERT(sum.x == 5 && sum.y == 7 && sum.z == 9, "Vec3 addition");
	Vec3 diff = b - a;
	TEST_ASSERT(diff.x == 3 && diff.y == 3 && diff.z == 3, "Vec3 subtraction");
	TEST_ASSERT(IsFloatEqual(dot(a, b), 32.f), "Vec3 dot product"); // 1*4+2*5+3*6=32
	Vec3 scaled = a * 2.f;
	TEST_ASSERT(scaled.x == 2 && scaled.y == 4 && scaled.z == 6, "Vec3 scale");
	TEST_ASSERT(a == a, "Vec3 equality");

	// Vec2
	Vec2 v2(1, 2);
	TEST_ASSERT(v2.x == 1 && v2.y == 2, "Vec2 construction");
	Vec2 v2n = normalize(v2);
	float len = v2n.length();
	TEST_ASSERT(IsFloatEqual(len, 1.f), "Vec2 normalize length");

	std::cout << "MathsTests: passed\n";
	return 0;
}
