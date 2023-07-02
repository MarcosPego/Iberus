#pragma once

#include "matrix.h"

namespace Math {
	struct Qtrn {
		float t, x, y, z;
		
		Qtrn();
		Qtrn(const float same_value);
		Qtrn(const float t, const float x, const float y, const float z);
		Qtrn(const Qtrn& q);

		Qtrn& operator += (const Qtrn& q);
		Qtrn& operator -= (const Qtrn& q);
		Qtrn& operator *= (const Qtrn& q);

		friend const Qtrn operator + (const Qtrn& q1, const Qtrn& q2);
		friend const Qtrn operator - (const Qtrn& q1, const Qtrn& q2);
		friend const Qtrn operator * (const float k, const Qtrn& q);
		friend const Qtrn operator * (const Qtrn& q, const float k);
		friend const Qtrn operator * (const Qtrn& q1, const Qtrn& q2);
		friend const Qtrn operator / (const Qtrn& q, const float k);

		friend const bool operator == (const Qtrn& q1, const Qtrn& q2);
		friend const bool operator !=  (const Qtrn& q1, const Qtrn& q2);
		friend std::ostream& operator << (std::ostream& os, const Qtrn& q);
		friend std::istream& operator >> (std::istream& is, Qtrn& q);

		//const float qThreshold = (float)1.0e-5;

		static const Qtrn qFromAngleAxis(float theta, Vec4 axis);
		static const void qToAngleAxis(const Qtrn& q, float& theta, Vec4& axis);

		friend const void qClean(Qtrn& q);
		friend const float qQuadrance(const Qtrn& q);
		friend const float qNorm(const Qtrn& q);
		friend const Qtrn qNormalize(const Qtrn& q);
		friend const Qtrn qConjugate(const Qtrn& q);
		friend const Qtrn qInverse(const Qtrn& q);
		friend const Qtrn qAdd(const Qtrn& q0, const Qtrn& q1);
		friend const Qtrn qMultiply(const Qtrn& q, const float s);
		friend const Qtrn qMultiply(const Qtrn& q0, const Qtrn& q1);
		friend const void qGLMatrix(const Qtrn& q, Mat4& matrix);
		friend const Qtrn qLerp(const Qtrn& q0, const Qtrn& q1, float k);
		friend const Qtrn qSlerp(const Qtrn& q0, const Qtrn& q1, float k);
		friend const bool qEqual(const Qtrn& q0, const Qtrn& q1);
		friend const void qPrint(const std::string& s, const Qtrn& q);
		friend const void qPrintAngleAxis(const std::string& s, const Qtrn& q);
	};
}