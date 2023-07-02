#include "Vector.h"

namespace Math {
	Vec4::Vec4() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 1;
	};

	Vec4::Vec4(const float same_value) {
		this->x = same_value;
		this->y = same_value;
		this->z = same_value;
		this->w = same_value;
	};

	Vec4::Vec4(const float x, const float y, const float z) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = 1;
	};

	Vec4::Vec4(const float x, const float y, const float z, const float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};

	Vec4::Vec4(const Vec2& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = 0;
		this->w = 1;
	};

	Vec4::Vec4(const Vec3& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = 1;
	};

	Vec4::Vec4(const Vec4& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	};

	const float  Vec4::quadrance() const {
		return this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
	};

	const float Vec4::length() const {
		return sqrt(this->quadrance());
	};

	const Vec4 operator - (const Vec4& v) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x == 0 ? v.x : -v.x;
		new_vector.y = v.y == 0 ? v.y : -v.y;
		new_vector.z = v.z == 0 ? v.z : -v.z;
		new_vector.w = v.w == 0 ? v.w : -v.w;
		return new_vector;
	};

	/*const Vec4 Vec4::operator - () const {
		Vec4 new_vector = Vec4();
		new_vector.x = this->x == 0 ? this->x : -this->x;
		new_vector.y = this->y == 0 ? this->y : -this->y;
		new_vector.z = this->z == 0 ? this->z : -this->z;
		new_vector.w = this->w == 0 ? this->w : -this->w;
		return new_vector;
	};*/

	Vec4& Vec4::operator+= (const Vec4& v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;
		return(*this);
	};

	Vec4& Vec4::operator-= (const Vec4& v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;
		return(*this);
	};

	Vec4& Vec4::operator*= (const Vec4& v) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;
		return(*this);
	};


	const Vec4 operator+ (const Vec4& v1, const Vec4& v2) {
		Vec4 new_vector = Vec4();
		new_vector += v1;
		new_vector += v2;

		return new_vector;
	};

	const Vec4 operator- (const Vec4& v1, const Vec4& v2) {
		Vec4 new_vector = Vec4();
		new_vector += v1;
		new_vector -= v2;

		return new_vector;
	};

	const Vec4 operator * (const float k, const Vec4& v) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;
		new_vector.z = v.z * k;
		new_vector.w = v.w * k;

		return new_vector;
	};

	const Vec4 operator * (const Vec4& v, const float k) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;
		new_vector.z = v.z * k;
		new_vector.w = v.w * k;

		return new_vector;
	};

	const Vec4 operator / (const float k, const Vec4& v) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;
		new_vector.z = v.z / k;
		new_vector.w = v.w / k;

		return new_vector;
	};

	const Vec4 operator / (const Vec4& v, const float k) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;
		new_vector.z = v.z / k;
		new_vector.w = v.w / k;

		return new_vector;
	};

	const float dot(const Vec4& v1, const Vec4& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	};

	const Vec4 normalize(const Vec4& v) {
		return v / v.length();
	};

	const bool operator == (const Vec4& v1, const Vec4& v2) {
		return fabs(v1.x - v2.x) < EPSILON && fabs(v1.y - v2.y) < EPSILON && fabs(v1.z - v2.z) < EPSILON && fabs(v1.w - v2.w) < EPSILON;
	};

	const bool operator !=  (const Vec4& v1, const Vec4& v2) {
		return fabs(v1.x - v2.x) > EPSILON || fabs(v1.y - v2.y) > EPSILON || fabs(v1.z - v2.z) > EPSILON || fabs(v1.w - v2.w) > EPSILON;
	};

	std::ostream& operator << (std::ostream& os, const Vec4& v) {
		os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
		return os;
	};

	std::istream& operator >> (std::istream& is, Vec4& v) {
		is >> v.x;
		is >> v.y;
		is >> v.z;
		is >> v.w;
		return is;
	};
}