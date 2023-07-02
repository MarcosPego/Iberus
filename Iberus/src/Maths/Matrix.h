#pragma once

#include "vector.h"

namespace Math {
	//Matrixes are used in Column major

	struct Mat4;

	struct Mat2 {
		GLfloat data[4];
		float array_size = (sizeof(data) / sizeof(*data));

		Mat2();
		Mat2(const float k);
		Mat2(const float m0, const float m1, 
			const float m2, const float m3);

		//Column major constructor
		Mat2(const Mat2& m);
		void clean();
		float determiant() const;
		
		Mat2& operator - (const Mat2& m);
		friend Mat2 adjugate(const Mat2& m);
		friend Mat2 transpose(const Mat2& m);
		friend Mat2 inverse(const Mat2& m);
		Mat2& operator += (const Mat2& m);
		Mat2& operator -= (const Mat2& m);
		Mat2& operator *= (const Mat2& m);
		friend const Mat2 operator + (const Mat2& m0, const Mat2& m1);
		friend const Mat2 operator - (const Mat2& m0, const Mat2& m1);
		friend const Mat2 operator * (const Mat2& m, const float k);
		friend const Mat2 operator * (const float k, const Mat2& m);
		friend const Vec2 operator * (const Mat2& m, const Vec2& v);
		friend const Mat2 operator * (const Mat2& m0, const Mat2& m1);

		friend const bool operator == (const Mat2& m0, const Mat2& m1);
		friend const bool operator != (const Mat2& m0, const Mat2& m1);
		friend std::istream& operator >> (std::istream& is, Mat2& m);
		friend std::ostream& operator << (std::ostream& os, const Mat2& m);
	};

	struct Mat3 {
		GLfloat data[9];
		float array_size = (sizeof(data) / sizeof(*data));

		Mat3();
		Mat3(const float k);
		Mat3(const float m0, const float m1, const float m2, 
			const float m3,  const float m4, const float m5, 
			const float m6, const float m7, const float m8);

		//Column major constructor
		Mat3(const Mat3& m);
		Mat3(const Mat4& m);

		void clean();
		float determiant() const;
		Mat3& operator - (const Mat3& m);
		friend Mat3 adjugate(const Mat3& m);
		friend Mat3 transpose(const Mat3& m);
		friend Mat3 inverse(const Mat3& m);
		Mat3& operator += (const Mat3& m);
		Mat3& operator -= (const Mat3& m);
		Mat3& operator *= (const Mat3& m);
		friend const Mat3 operator + (const Mat3& m0, const Mat3& m1);
		friend const Mat3 operator - (const Mat3& m0, const Mat3& m1);
		friend const Mat3 operator * (const Mat3& m, const float k);
		friend const Mat3 operator * (const float k, const Mat3& m);
		friend const Vec3 operator * (const Mat3& m, const Vec3& v);
		friend const Mat3 operator * (const Mat3& m0, const Mat3& m1);

		friend const bool operator == (const Mat3& m0, const Mat3& m1);
		friend const bool operator != (const Mat3& m0, const Mat3& m1);
		friend std::istream& operator >> (std::istream& is, Mat3& m);
		friend std::ostream& operator << (std::ostream& os, const Mat3& m);

	};

	struct Mat4 {
		GLfloat data[16];
		float array_size = (sizeof(data) / sizeof(*data));

		Mat4();
		Mat4(const float k);
		Mat4(const float m0, const float m1, const float m2, const float m3, 
			const float m4, const float m5, const float m6, const float m7, 
			const float m8, const float m9, const float m10, const float m11, 
			const float m12, const float m13, const float m14, const float m15);

		//Column major constructor
		Mat4(const Mat3& m);
		Mat4(const Mat4& m);
		void clean();
		Mat4& operator - (const Mat4& m);

		friend Mat4 transpose(const Mat4& m);
		friend Mat4 inverse(const Mat4& m);
		Mat4& operator += (const Mat4& m);
		Mat4& operator -= (const Mat4& m);
		Mat4& operator *= (const Mat4& m);
		friend const Mat4 operator + (const Mat4& m0, const Mat4& m1);
		friend const Mat4 operator - (const Mat4& m0, const Mat4& m1);
		friend const Mat4 operator * (const Mat4& m, const float k);
		friend const Mat4 operator * (const float k, const Mat4& m);
		friend const Vec4 operator * (const Mat4& m, const Vec4& v);
		friend const Mat4 operator * (const Mat4& m0, const Mat4& m1);

		friend const bool operator == (const Mat4& m0, const Mat4& m1);
		friend const bool operator != (const Mat4& m0, const Mat4& m1);
		friend std::istream& operator >> (std::istream& is, Mat4& m);;
		friend std::ostream& operator << (std::ostream& os, const Mat4& m);

		
	};

	struct MatrixFactory {
		static const Mat2 CreateZeroMat2();
		static const Mat2 CreateIdentityMat2();
		static const Mat2 CreateScaleMat2(const Vec2& v);
		static const Mat2 CreateRotationMat2(const float angle);
						  
		static const Mat3 CreateZeroMat3();
		static const Mat3 CreateIdentityMat3();
		static const Mat3 CreateDualMat3(const Vec3& v);
		static const Mat3 CreateScaleMat3(const Vec3& v);
		static const Mat3 CreateRotationMat3(const float angle, const Vec3& axis);
		static const Mat3 CreateMat3FromMat4(const Mat4& axis);
						  
		static const Mat3 CreateRandomBet9Mat3();
						  
		static const Mat4 CreateZeroMat4();
		static const Mat4 CreateIdentityMat4();
		static const Mat4 CreateScaleMat4(const Vec3& v);
		static const Mat4 CreateRotationMat4(const float angle, const Vec3& axis);
		static const Mat4 CreateTranslationMat4( const Vec3& vector);
		static const Mat4 CreateMat4FromMat3(const Mat3& m3);
						  
		static const Mat4 CreateRotation2DMat4(const float angle);
						  
		static const Mat4 CreateViewMat4(const Vec3& eye, const Vec3& center, const Vec3& up);
		static const Mat4 CreateOrtoMat4(const float left, const float right, const float bottom, const float top, const float near, const float far);
		static const Mat4 CreatePerspectiveMat4(const float fovy, const float aspect, const float nearZ, const float farZ);
						  
		static const Mat4 CreateModelMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale,
			const Vec3& parentPos, const Vec3& parentRot, const Vec3& parentScale);
						  
		static const Mat4 CreateReflectionMatrix(Vec4 plane_vec);
		static const Mat4 CreateNEGY();
	};
};