#pragma once

#include "Core.h"

namespace Math {
	static float EPSILON = 0.00001f;
	struct Vec4;
	struct Vec3;

	struct IBERUS_API Vec2 {
		union {
			struct { float x, y; };
			float data[2];
		};

		Vec2();
		Vec2(const float same_value);
		Vec2(const float x, const float y);
		Vec2(const Vec2& Vec2);
		Vec2(const Vec3& Vec3);
		//float* data();
		//void clean();
		const float quadrance() const;
		const float length() const;
		friend const Vec2 operator - (const Vec2& v);
		//const Vec2 operator - () const;
		Vec2& operator += (const Vec2& v);
		Vec2& operator -= (const Vec2& v);
		Vec2& operator *= (const Vec2& v);
		friend const Vec2 normalize(const Vec2& v);
		friend const Vec2 operator + (const Vec2& v1, const Vec2& v2);
		friend const Vec2 operator - (const Vec2& v1, const Vec2& v2);
		friend const Vec2 operator * (const float k, const Vec2& v);
		friend const Vec2 operator * (const Vec2& v, const float k);
		friend const Vec2 operator / (const float k, const Vec2& v);
		friend const Vec2 operator / (const Vec2& v, const float k);
		friend const float dot(const Vec2& v1, const Vec2& v2);
		friend const bool operator == (const Vec2& v1, const Vec2& v2);
		friend const bool operator !=  (const Vec2& v1, const Vec2& v2);
		friend std::ostream& operator << (std::ostream& os, const Vec2& v);
		friend std::istream& operator >> (std::istream& is, Vec2& v);
	};

	struct IBERUS_API Vec3 {
		union {
			struct { float x, y, z; };
			float data[3];
		};

		Vec3();
		Vec3(const float same_value);
		Vec3(const float x, const float y);
		Vec3(const float x, const float y, const float z);
		Vec3(const Vec2& Vec2);
		Vec3(const Vec3& Vec3);
		Vec3(const Vec4& Vec4);

		//float* data();
		//void clean();
		const float quadrance() const;
		const float length() const;
		friend const Vec3 operator - (const Vec3& v);
		//const Vec3 operator - () const;
		Vec3& operator += (const Vec3& v);
		Vec3& operator -= (const Vec3& v);
		Vec3& operator *= (const Vec3& v);
		friend IBERUS_API const Vec3 normalize(const Vec3& v);
		friend IBERUS_API const Vec3 operator + (const Vec3& v1, const Vec3& v2);
		friend IBERUS_API const Vec3 operator - (const Vec3& v1, const Vec3& v2);
		friend IBERUS_API const Vec3 operator * (const float k, const Vec3& v);
		friend IBERUS_API const Vec3 operator * (const Vec3& v, const float k);
		friend IBERUS_API const Vec3 operator / (const float k, const Vec3& v);
		friend IBERUS_API const Vec3 operator / (const Vec3& v, const float k);
		friend IBERUS_API const float dot(const Vec3& v1, const Vec3& v2);
		friend IBERUS_API const Vec3 cross(const Vec3& v1, const Vec3& v2);
		friend IBERUS_API const bool operator == (const Vec3& v1, const Vec3& v2);
		friend IBERUS_API const bool operator !=  (const Vec3& v1, const Vec3& v2);
		friend IBERUS_API std::ostream& operator << (std::ostream& os, const Vec3& v);
		friend IBERUS_API std::istream& operator >> (std::istream& is, Vec3& v);
	};

	struct IBERUS_API Vec4 {
		union {
			struct { float x, y, z, w; };
			float data[4];
		};


		Vec4();
		Vec4(const float same_value);
		Vec4(const float x, const float y, const float z);
		Vec4(const float x, const float y, const float z, const float w);
		Vec4(const Vec2& v);
		Vec4(const Vec3& v);
		Vec4(const Vec4& v);
		//float* data();
		//void clean();

		const float quadrance() const;
		const float length() const;
		friend const Vec4 operator - (const Vec4& v);
		//const Vec4 operator - () const;
		Vec4& operator += (const Vec4& v);
		Vec4& operator -= (const Vec4& v);
		Vec4& operator *= (const Vec4& v);
		friend const Vec4 normalize(const Vec4& v);
		friend const Vec4 operator + (const Vec4& v1, const Vec4& v2);
		friend const Vec4 operator - (const Vec4& v1, const Vec4& v2);
		friend const Vec4 operator * (const float k, const Vec4& v);
		friend const Vec4 operator * (const Vec4& v, const float k);
		friend const Vec4 operator / (const float k, const Vec4& v);
		friend const Vec4 operator / (const Vec4& v, const float k);
		friend const float dot(const Vec4& v1, const Vec4& v2);

		friend const bool operator == (const Vec4& v1, const Vec4& v2);
		friend const bool operator !=  (const Vec4& v1, const Vec4& v2);
		friend std::ostream& operator << (std::ostream& os, const Vec4& v);
		friend std::istream& operator >> (std::istream& is, Vec4& v);
	};
};