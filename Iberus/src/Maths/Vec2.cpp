#include "Vector.h"


namespace Math {

	Vec2::Vec2() {
		this->x = 0;
		this->y = 0;
	};

	Vec2::Vec2(const float same_value) {
		this->x = same_value;
		this->y = same_value;
	};

	Vec2::Vec2(const float x, const float y) {
		this->x = x;
		this->y = y;
	};

	Vec2::Vec2(const Vec2& Vec2) {
		this->x = Vec2.x;
		this->y = Vec2.y;
	};

	Vec2::Vec2(const Vec3& Vec3){
		this->x = Vec3.x / Vec3.z;
		this->y = Vec3.y / Vec3.z;
	}

	const float  Vec2::quadrance() const {
		return this->x * this->x + this->y * this->y;
	};

	const float Vec2::length() const {
		return sqrt(this->quadrance());
	};

	const Vec2 operator - (const Vec2& v) {
		Vec2 new_vector = Vec2();
		new_vector.x = v.x == 0 ? v.x : -v.x;
		new_vector.y = v.y == 0 ? v.y : -v.y;
		
		return new_vector;
	};

	/*const Vec2 Vec2::operator - () const {
		Vec2 new_vector = Vec2();
		new_vector.x = this->x == 0 ? this->x : -this->x;
		new_vector.y = this->y == 0 ? this->y : -this->y;
		return new_vector;
	};*/

	Vec2& Vec2::operator+= (const Vec2& v) {
		this->x += v.x;
		this->y += v.y;
		return(*this);
	};

	Vec2& Vec2::operator-= (const Vec2& v) {
		this->x -= v.x;
		this->y -= v.y;
		return(*this);
	};

	Vec2& Vec2::operator*= (const Vec2& v) {
		this->x *= v.x;
		this->y *= v.y;
		return(*this);
	};

	const Vec2 operator+ (const Vec2& v1, const Vec2& v2) {
		Vec2 new_vector = Vec2();
		new_vector += v1;
		new_vector += v2;

		return new_vector;
	};

	const Vec2 operator- (const Vec2& v1, const Vec2& v2) {
		Vec2 new_vector = Vec2();
		new_vector += v1;
		new_vector -= v2;

		return new_vector;
	};

	const Vec2 operator * (const float k, const Vec2& v) {
		Vec2 new_vector = Vec2();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;

		return new_vector;
	};

	const Vec2 operator * (const Vec2& v, const float k) {
		Vec2 new_vector = Vec2();
		new_vector.x = v.x * k;
		new_vector.y = v.y * k;

		return new_vector;
	};

	const Vec2 operator / (const float k, const Vec2& v) {
		Vec2 new_vector = Vec2();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;

		return new_vector;
	};

	const Vec2 operator / (const Vec2& v, const float k) {
		Vec2 new_vector = Vec2();
		new_vector.x = v.x / k;
		new_vector.y = v.y / k;

		return new_vector;
	};

	const float dot(const Vec2& v1, const Vec2& v2) {
		return v1.x*v2.x + v1.y*v2.y;
	};


	const Vec2 normalize(const Vec2& v) {
		return v / v.length();
	};

	const bool operator == (const Vec2& v1, const Vec2& v2) {
		return fabs(v1.x - v2.x) < EPSILON && fabs(v1.y - v2.y) < EPSILON;
	};
	
	const bool operator !=  (const Vec2& v1, const Vec2& v2) {
		return fabs(v1.x - v2.x) > EPSILON || fabs(v1.y - v2.y) > EPSILON;
	};

	std::ostream& operator << (std::ostream& os, const Vec2& v) {
		os << "(" << v.x << "," << v.y <<  ")";
		return os;
	}; 

	std::istream& operator >> (std::istream& is, Vec2& v) {
		is >> v.x;
		is >> v.y;
		return is;
	};
};