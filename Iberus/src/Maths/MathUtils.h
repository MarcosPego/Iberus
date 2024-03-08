#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector.h"
#include "Matrix.h"
#include "Quaternions.h"
#include "Noise.h"

namespace Math {
#define DEGREES_TO_RADIANS 0.01745329251994329547
#define RADIANS_TO_DEGREES 57.29577951308232185913
#define EPSILON (float)1.0e-5

static bool IsFloatEqual(float val, float target)
{
	return val < target + EPSILON && val > target - EPSILON;
}

static float Deg2Rad(float degrees)
{
	return (float)(degrees * (M_PI / 180.0));
}

static float Rad2Deg(float radians)
{
	return (float)(radians / (M_PI / 180.0));
}
}