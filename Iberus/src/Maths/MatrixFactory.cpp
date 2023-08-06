#include "Enginepch.h"
#include "matrix.h"

#undef near
#undef far

namespace Math {
	const Mat2 MatrixFactory::CreateZeroMat2() {
		return Mat2();
	}
	const Mat2 MatrixFactory::CreateIdentityMat2() {
		return Mat2(1,0,0,1);
	}
	const Mat2 MatrixFactory::CreateScaleMat2(const Vec2& v) {
		return Mat2(v.x, 0, 0, v.y);
	}
	const Mat2 MatrixFactory::CreateRotationMat2(const float angle) {
		return Mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
	}

	const Mat3 MatrixFactory::CreateZeroMat3() {
		return Mat3();
	}
	const Mat3 MatrixFactory::CreateIdentityMat3() {
		return Mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
	}

	const Mat3 MatrixFactory::CreateDualMat3(const Vec3& v) {
		return Mat3(0, v.z, -v.y,-v.z, 0, v.x, v.y, -v.x, 0);
	}

	const Mat3 MatrixFactory::CreateScaleMat3(const Vec3& v) {
		return Mat3(v.x, 0, 0, 0 , v.y, 0, 0, 0, v.y);
	}
	const Mat3 MatrixFactory::CreateRotationMat3(const float angle, const Vec3& axis) {
		Vec3 normalized = normalize(axis);
		float c = cos(angle);
		float s = sin(angle);
		float ux = axis.x;
		float uy = axis.y;
		float uz = axis.z;
		return Mat3( c + ux * ux * (1-c) , uy * ux * (1 - c) + uz * s, ux * uz * (1 - c) - uy * s,
					ux * uy * (1 - c) - uz * s, c + uy * uy * (1 - c), uz * uy * (1 - c) + ux * s,
					ux * uz * (1 - c) + uy * s, uy * uz * (1 - c) - ux * s, c + uz * uz * (1 - c));
	}


	const Mat3 MatrixFactory::CreateMat3FromMat4(const Mat4& m4) {
		return Mat3(m4.data[0], m4.data[1], m4.data[2],
					m4.data[4], m4.data[5], m4.data[6],
					m4.data[8], m4.data[9], m4.data[10]	
			);
	}

	const Mat3 MatrixFactory::CreateRandomBet9Mat3() {
		Mat3 m = Mat3();
		for (int i = 0; i < m.array_size; i++) {
			m.data[i] = static_cast<float>(rand() % 19 - 9);
		}
		return m;
	}

	const Mat4 MatrixFactory::CreateZeroMat4() {
		return Mat4();
	}
	const Mat4 MatrixFactory::CreateIdentityMat4() {
		return Mat4(1, 0, 0, 0, 
					0, 1, 0, 0, 
					0, 0, 1, 0, 
					0, 0, 0, 1);
	}

	const Mat4 MatrixFactory::CreateScaleMat4(const Vec3& v) {
		return Mat4(v.x, 0, 0, 0, 
					0, v.y, 0, 0,
					0, 0, v.z, 0,
					0, 0, 0,  1);
	}
	const Mat4 MatrixFactory::CreateRotationMat4(const float angle, const Vec3& axis) {
		Vec3 normalized = normalize(axis);
		float c = cos(angle);
		float s = sin(angle);
		float ux = axis.x;
		float uy = axis.y;
		float uz = axis.z;
		return Mat4(
			c + ux * ux * (1 - c), uy * ux * (1 - c) + uz * s, ux * uz * (1 - c) - uy * s, 0, 
			ux * uy * (1 - c) - uz * s, c + uy * uy * (1 - c), uz * uy * (1 - c) + ux * s, 0,
			ux * uz * (1 - c) + uy * s, uy * uz * (1 - c) - ux * s, c + uz * uz * (1 - c), 0,
			0, 0, 0, 1);
	}

	const Mat4 MatrixFactory::CreateRotation2DMat4(const float angle) {
		float c = cos(angle);
		float s = sin(angle);
		return Mat4(
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 1);
	}

	const Mat4 MatrixFactory::CreateTranslationMat4(const Vec3& vector) {
		return Mat4(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					vector.x, vector.y, vector.z, 1);
	}

	const Mat4 MatrixFactory::CreateMat4FromMat3(const Mat3& m3) {
		return Mat4(m3.data[0], m3.data[1], m3.data[2], 0,
					m3.data[3], m3.data[4], m3.data[5], 0,
					m3.data[6], m3.data[7], m3.data[8], 0,
					0, 0, 0, 1
		);
	}



	const Mat4 MatrixFactory::CreateViewMat4(const Vec3& eye, const Vec3& center, const Vec3& up) {
		Vec3 forward = normalize(center - eye);
		Vec3 right = normalize(cross(forward, up));
		Vec3 v = cross(right, forward);
		//forward = -forward;
		return Mat4(right.x, v.x, -forward.x, 0,
					right.y, v.y, -forward.y, 0,
					right.z, v.z, -forward.z, 0,
					-dot(right, eye), -dot(v, eye), dot(forward, eye), 1
		);
	}


	const Mat4 MatrixFactory::CreateOrtoMat4(const float left, const float right, const float bottom, const float top, const float near, const float far) {
		return Mat4(2/ (right - left), 0, 0, 0,
			0, 2/ (top - bottom), 0, 0,
			0, 0, -2 / (far - near), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1
		);
	}


	const Mat4 MatrixFactory::CreatePerspectiveMat4(const float fovy, const float aspect, const float nearZ, const float farZ) {
		return Mat4( 1/ (aspect * tan(fovy/2 * M_PI / 180.0)), 0, 0, 0,
					0, 1 / tan(fovy / 2 * M_PI / 180.0), 0, 0,
					0, 0, -(farZ + nearZ) / (farZ - nearZ), -1,
					0, 0, -(2 * farZ * nearZ) / (farZ - nearZ), 0
		);
	}

	const Mat4 MatrixFactory::CreateModelMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec3& parentPos, const Vec3& parentRot, const Vec3& parentScale) {
		return MatrixFactory::CreateTranslationMat4(parentPos) *
			MatrixFactory::CreateRotationMat4(parentRot.x, Vec3(1, 0, 0)) *
			MatrixFactory::CreateRotationMat4(parentRot.y, Vec3(0, 1, 0)) *
			MatrixFactory::CreateRotationMat4(parentRot.z, Vec3(0, 0, 1)) *

			MatrixFactory::CreateTranslationMat4(position) *
			MatrixFactory::CreateRotationMat4(rotation.x, Vec3(1, 0, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.y, Vec3(0, 1, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.z, Vec3(0, 0, 1)) *
			MatrixFactory::CreateScaleMat4(scale);
	}

	const Mat4 MatrixFactory::CreateModelMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Mat4& parentMatrix) {
		return parentMatrix * MatrixFactory::CreateTranslationMat4(position) *
			MatrixFactory::CreateRotationMat4(rotation.x, Vec3(1, 0, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.y, Vec3(0, 1, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.z, Vec3(0, 0, 1)) *
			MatrixFactory::CreateScaleMat4(scale);
	}

	const Mat4 MatrixFactory::CreateModelMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale) {
		return MatrixFactory::CreateTranslationMat4(position) *
			MatrixFactory::CreateRotationMat4(rotation.x, Vec3(1, 0, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.y, Vec3(0, 1, 0)) *
			MatrixFactory::CreateRotationMat4(rotation.z, Vec3(0, 0, 1)) *
			MatrixFactory::CreateScaleMat4(scale);
	}

	const Mat4 MatrixFactory::CreateReflectionMatrix(Vec4 plane_vec) {
		float a = plane_vec.x;
		float b = plane_vec.y;
		float c = plane_vec.z;
		float d = plane_vec.w;

		return Mat4(1 - 2 * a * a, -2 * a * b, -2 * a * c, 0,
			-2 * a * b, 1 - 2 * b * b, -2 * b * c, 0,
			-2 * a * c, -2 * b * c, 1 - 2 * c * c, 0,
			-2 * a * d, -2 * b * d, -2 * c * d, 1);
	}


	const Mat4 MatrixFactory::CreateNEGY() {

		return Mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1);
	}
}