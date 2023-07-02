#include "Enginepch.h"
#include "Vector.h"

namespace Math {
	Vec3::Vec3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	};

	Vec3::Vec3(const float same_value) {
		this->x = same_value;
		this->y = same_value;
		this->z = same_value;
	};

	Vec3::Vec3(const float x, const float y) {
		this->x = x;
		this->y = y;
		this->z = 0;
	};

	Vec3::Vec3(const float x, const float y, const float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	};

	Vec3::Vec3(const Vec2& Vec2) {
		this->x = Vec2.x;
		this->y = Vec2.y;
		this->z = 0;
	};

	Vec3::Vec3(const Vec3& Vec3) {
		this->x = Vec3.x;
		this->y = Vec3.y;
		this->z = Vec3.z;
	};

	Vec3::Vec3(const Vec4& Vec4) {
		this->x = Vec4.x / Vec4.w;
		this->y = Vec4.y / Vec4.w;
		this->z = Vec4.z / Vec4.w;
	};

	const float  Vec3::quadrance() const {
		return this->x * this->x + this->y * this->y + this->z * this->z;
	};

	const float Vec3::length() const {
		return sqrt(this->quadrance());
	};

	const Vec3 operator - (const Vec3& v) {
		Vec3 new_vector = Vec3();
		new_vector.x = v.x == 0 ? v.x : -v.x;
		new_vector.y = v.y == 0 ? v.y : -v.y;
		new_vector.z = v.z == 0 ? v.z : -v.z;
		return new_vector;
	};

	/*const Vec3 Vec3::operator - () const {
		Vec3 new_vector = Vec3();
		new_vector.x = this->x == 0 ? this->x : -this->x;
		new_vector.y = this->y == 0 ? this->y : -this->y;
		new_vector.z = this->z == 0 ? this->z : -this->z;
		return new_vector;
	};*/

	Vec3& Vec3::operator+= (const Vec3& v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return(*this);
	};

	Vec3& Vec3::operator-= (const Vec3& v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return(*this);
	};

	Vec3& Vec3::operator*= (const Vec3& v) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return(*this);
	};


	const Vec3 operator+ (const Vec3& v1, const Vec3& v2) {
		Vec3 new_vector = Vec3();
		new_vector += v1;
		new_vector += v2;

		return new_vector;
	};

	const Vec3 operator- (const Vec3& v1, const Vec3& v2) {
		Vec3 new_vector = Vec3();
		new_vector += v1;
		new_vector -= v2;

		return new_vector;
	};

	const Vec3 operator * (const float k, const Vec3& v) {
		Vec3 new_vector = Vec3();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;
		new_vector.z = v.z * k;

		return new_vector;
	};

	const Vec3 operator * (const Vec3& v, const float k) {
		Vec3 new_vector = Vec3();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;
		new_vector.z = v.z * k;

		return new_vector;
	};

	const Vec3 operator / (const float k, const Vec3& v) {
		Vec3 new_vector = Vec3();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;
		new_vector.z = v.z / k;

		return new_vector;
	};

	const Vec3 operator / (const Vec3& v, const float k) {
		Vec3 new_vector = Vec3();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;
		new_vector.z = v.z / k;

		return new_vector;
	};

	const float dot(const Vec3& v1, const Vec3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	};

	const Vec3 cross(const Vec3& v1, const Vec3& v2) {
		Vec3 new_vector = Vec3();
		new_vector.x = v1.y* v2.z  - v1.z* v2.y;
		new_vector.y = v1.z * v2.x - v1.x * v2.z;
		new_vector.z = v1.x * v2.y - v1.y * v2.x;
		
		return new_vector;
	};

	const Vec3 normalize(const Vec3& v) {
		return v / v.length();
	};



	const bool operator == (const Vec3& v1, const Vec3& v2) {
		return fabs(v1.x - v2.x) < EPSILON && fabs(v1.y - v2.y) < EPSILON && fabs(v1.z - v2.z) < EPSILON;
	};

	const bool operator !=  (const Vec3& v1, const Vec3& v2) {
		return fabs(v1.x - v2.x) > EPSILON || fabs(v1.y - v2.y) > EPSILON || fabs(v1.z - v2.z) > EPSILON;
	};

	std::ostream& operator << (std::ostream& os, const Vec3& v) {
		os << "(" << v.x << "," << v.y << "," << v.z << ")";
		return os;
	};

	std::istream& operator >> (std::istream& is, Vec3& v) {
		is >> v.x;
		is >> v.y;
		is >> v.z;
		return is;
	};
};