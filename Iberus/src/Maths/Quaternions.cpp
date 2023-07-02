#include "Enginepch.h"
#include "MathUtils.h"

namespace Math {
	Qtrn::Qtrn() {
		this->t = 1;
		this->x = 0;
		this->y = 0;
		this->z = 0;
	};

	Qtrn::Qtrn(const float same_value) {
		this->t = same_value;
		this->x = same_value;
		this->y = same_value;
		this->z = same_value;
	};

	Qtrn::Qtrn(const float t, const float x, const float y, const float z) {
		this->t = t;
		this->x = x;
		this->y = y;
		this->z = z;
	};

	Qtrn::Qtrn(const Qtrn& q) {
		this->t = q.t;
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
	};

	Qtrn& Qtrn::operator+= (const Qtrn& q) {
		this->t+= q.t;
		this->x += q.x;
		this->y += q.y;
		this->z += q.z;
		return(*this);
	};

	Qtrn& Qtrn::operator-= (const Qtrn& q) {
		this->t -= q.t;
		this->x -= q.x;
		this->y -= q.y;
		this->z -= q.z;
		return(*this);
	};

	Qtrn& Qtrn::operator*= (const Qtrn& q1) {
		this->t = this->t * q1.t - this->x * q1.x - this->y * q1.y - this->z * q1.z;
		this->x = this->t * q1.x + this->x * q1.t + this->y * q1.z - this->z * q1.y;
		this->y = this->t * q1.y + this->y * q1.t + this->z * q1.x - this->x * q1.z;
		this->z = this->t * q1.z + this->z * q1.t + this->x * q1.y - this->y * q1.x;
		return(*this);
	};


	const Qtrn operator+ (const Qtrn& q1, const Qtrn& q2) {
		return qAdd(q1, q2);
	};

	const Qtrn operator- (const Qtrn& q1, const Qtrn& q2) {
		Qtrn q;
		q.t = q1.t - q2.t;
		q.x = q1.x - q2.x;
		q.y = q1.y - q2.y;
		q.z = q1.z - q2.z;
		return q;
	};

	const Qtrn operator* (const float k, const Qtrn& q1) {
		return qMultiply(q1, k);
	};

	const Qtrn operator* (const Qtrn& q1, const float k) {
		return qMultiply(q1, k);
	};

	const Qtrn operator* (const Qtrn& q1, const Qtrn& q2) {
		return qMultiply(q1, q2);
	}

	const Qtrn operator/ (const Qtrn& q1, const float k) {
		return qMultiply(q1, 1/k);
	};


	const bool operator == (const Qtrn& q0, const Qtrn& q1) {
		return qEqual(q0, q1);
	};

	const bool operator !=  (const Qtrn& q0, const Qtrn& q1) {
		return !qEqual(q0, q1);
	};

	std::ostream& operator << (std::ostream& os, const Qtrn& q) {
		os << "(" << q.t << ", " << q.x << ", " << q.y << ", " << q.z << ")";
		return os;
	};

	std::istream& operator >> (std::istream& is, Qtrn& q) {
		is >> q.t;
		is >> q.x;
		is >> q.y;
		is >> q.z;
		return is;
	};
	

	const Qtrn Qtrn::qFromAngleAxis(float theta, Vec4 axis)
	{
		Vec4 axisn = normalize(axis);

		Qtrn q;
		float a = theta * (float)DEGREES_TO_RADIANS;
		q.t = cos(a / 2.0f);
		float s = sin(a / 2.0f);
		q.x = axisn.x * s;
		q.y = axisn.y * s;
		q.z = axisn.z * s;

		qClean(q);
		return qNormalize(q);
	}

	const void  Qtrn::qToAngleAxis(const Qtrn& q, float& theta, Vec4& axis)
	{
		Qtrn qn = qNormalize(q);
		theta = 2.0f * acos(qn.t) * (float)RADIANS_TO_DEGREES;
		float s = sqrt(1.0f - qn.t * qn.t);
		if (s < EPSILON) {
			axis.x = 1.0f;
			axis.y = 0.0f;
			axis.z = 0.0f;
			axis.w = 1.0f;
		}
		else {
			float sinv = 1 / s;
			axis.x = qn.x * sinv;
			axis.y = qn.y * sinv;
			axis.z = qn.z * sinv;
			axis.w = 1.0f;
		}
	}

	const void qClean(Qtrn& q)
	{
		if (fabs(q.t) < EPSILON) q.t = 0.0f;
		if (fabs(q.x) < EPSILON) q.x = 0.0f;
		if (fabs(q.y) < EPSILON) q.y = 0.0f;
		if (fabs(q.z) < EPSILON) q.z = 0.0f;
	}

	const float qQuadrance(const Qtrn& q)
	{
		return q.t * q.t + q.x * q.x + q.y * q.y + q.z * q.z;
	}

	const float qNorm(const Qtrn& q)
	{
		return sqrt(qQuadrance(q));
	}

	const Qtrn qNormalize(const Qtrn& q)
	{
		float s = 1 / qNorm(q);
		return qMultiply(q, s);
	}

	const Qtrn qConjugate(const Qtrn& q)
	{
		Qtrn qconj = { q.t, -q.x, -q.y, -q.z };
		return qconj;
	}

	const Qtrn qInverse(const Qtrn& q)
	{
		return qMultiply(qConjugate(q), 1.0f / qQuadrance(q));
	}

	const Qtrn qAdd(const Qtrn& q0, const Qtrn& q1)
	{
		Qtrn q;
		q.t = q0.t + q1.t;
		q.x = q0.x + q1.x;
		q.y = q0.y + q1.y;
		q.z = q0.z + q1.z;
		return q;
	}

	const Qtrn qMultiply(const Qtrn& q, const float s)
	{
		Qtrn sq;
		sq.t = s * q.t;
		sq.x = s * q.x;
		sq.y = s * q.y;
		sq.z = s * q.z;
		return sq;
	}

	const Qtrn qMultiply(const Qtrn& q0, const Qtrn& q1)
	{
		Qtrn q;
		q.t = q0.t * q1.t - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z;
		q.x = q0.t * q1.x + q0.x * q1.t + q0.y * q1.z - q0.z * q1.y;
		q.y = q0.t * q1.y + q0.y * q1.t + q0.z * q1.x - q0.x * q1.z;
		q.z = q0.t * q1.z + q0.z * q1.t + q0.x * q1.y - q0.y * q1.x;
		return q;
	}

	const void qGLMatrix(const Qtrn& q, Mat4& matrix)
	{
		Qtrn qn = qNormalize(q);

		float xx = qn.x * qn.x;
		float xy = qn.x * qn.y;
		float xz = qn.x * qn.z;
		float xt = qn.x * qn.t;
		float yy = qn.y * qn.y;
		float yz = qn.y * qn.z;
		float yt = qn.y * qn.t;
		float zz = qn.z * qn.z;
		float zt = qn.z * qn.t;

		matrix.data[0] = 1.0f - 2.0f * (yy + zz);
		matrix.data[1] = 2.0f * (xy + zt);
		matrix.data[2] = 2.0f * (xz - yt);
		matrix.data[3] = 0.0f;

		matrix.data[4] = 2.0f * (xy - zt);
		matrix.data[5] = 1.0f - 2.0f * (xx + zz);
		matrix.data[6] = 2.0f * (yz + xt);
		matrix.data[7] = 0.0f;

		matrix.data[8] = 2.0f * (xz + yt);
		matrix.data[9] = 2.0f * (yz - xt);
		matrix.data[10] = 1.0f - 2.0f * (xx + yy);
		matrix.data[11] = 0.0f;

		matrix.data[12] = 0.0f;
		matrix.data[13] = 0.0f;
		matrix.data[14] = 0.0f;
		matrix.data[15] = 1.0f;

		matrix.clean();
	}

	const Qtrn qLerp(const Qtrn& q0, const Qtrn& q1, float k)
	{
		float cos_angle = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.t * q1.t;
		float k0 = 1.0f - k;
		float k1 = (cos_angle > 0) ? k : -k;
		Qtrn qi = qAdd(qMultiply(q0, k0), qMultiply(q1, k1));
		return qNormalize(qi);
	}

	const Qtrn qSlerp(const Qtrn& q0, const Qtrn& q1, float k)
	{
		float angle = acos(q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.t * q1.t);
		float k0 = sin((1 - k) * angle) / sin(angle);
		float k1 = sin(k * angle) / sin(angle);
		Qtrn qi = qAdd(qMultiply(q0, k0), qMultiply(q1, k1));
		return qNormalize(qi);
	}

	const bool qEqual(const Qtrn& q0, const Qtrn& q1)
	{
		return (fabs(q0.t - q1.t) < EPSILON && fabs(q0.x - q1.x) < EPSILON &&
			fabs(q0.y - q1.y) < EPSILON && fabs(q0.z - q1.z) < EPSILON);
	}

	const void qPrint(const std::string& s, const Qtrn& q)
	{
		std::cout << s << " = (" << q.t << ", " << q.x << ", " << q.y << ", " << q.z << ")" << std::endl;
	}

	const void qPrintAngleAxis(const std::string& s, const Qtrn& q)
	{
		std::cout << s << " = [" << std::endl;

		float thetaf;
		Vec4 axis_f;
		Qtrn::qToAngleAxis(q, thetaf, axis_f);
		std::cout << "  angle = " << thetaf << std::endl;
		std::cout << "  axis"<< axis_f << std::endl;
		std::cout << "]" << std::endl;
	}
}