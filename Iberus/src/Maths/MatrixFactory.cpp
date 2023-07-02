#include "matrix.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace cgj {
	const mat2 MatrixFactory::createZeroMat2() {
		return mat2();
	}
	const mat2 MatrixFactory::createIdentityMat2() {
		return mat2(1,0,0,1);
	}
	const mat2 MatrixFactory::createScaleMat2(const Vec2& v) {
		return mat2(v.x, 0, 0, v.y);
	}
	const mat2 MatrixFactory::createRotationMat2(const float angle) {
		return mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
	}

	const mat3 MatrixFactory::createZeroMat3() {
		return mat3();
	}
	const mat3 MatrixFactory::createIdentityMat3() {
		return mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
	}

	const mat3 MatrixFactory::createDualMat3(const Vec3& v) {
		return mat3(0, v.z, -v.y,-v.z, 0, v.x, v.y, -v.x, 0);
	}

	const mat3 MatrixFactory::createScaleMat3(const Vec3& v) {
		return mat3(v.x, 0, 0, 0 , v.y, 0, 0, 0, v.y);
	}
	const mat3 MatrixFactory::createRotationMat3(const float angle, const Vec3& axis) {
		Vec3 normalized = normalize(axis);
		float c = cos(angle);
		float s = sin(angle);
		float ux = axis.x;
		float uy = axis.y;
		float uz = axis.z;
		return mat3( c + ux * ux * (1-c) , uy * ux * (1 - c) + uz * s, ux * uz * (1 - c) - uy * s,
					ux * uy * (1 - c) - uz * s, c + uy * uy * (1 - c), uz * uy * (1 - c) + ux * s,
					ux * uz * (1 - c) + uy * s, uy * uz * (1 - c) - ux * s, c + uz * uz * (1 - c));
	}


	const mat3 MatrixFactory::createMat3FromMat4(const mat4& m4) {
		return mat3(m4.data[0], m4.data[1], m4.data[2],
					m4.data[4], m4.data[5], m4.data[6],
					m4.data[8], m4.data[9], m4.data[10]	
			);
	}

	const mat3 MatrixFactory::createRandomBet9Mat3() {
		mat3 m = mat3();
		for (int i = 0; i < m.array_size; i++) {
			m.data[i] = static_cast<float>(rand() % 19 - 9);
		}
		return m;
	}

	const mat4 MatrixFactory::createZeroMat4() {
		return mat4();
	}
	const mat4 MatrixFactory::createIdentityMat4() {
		return mat4(1, 0, 0, 0, 
					0, 1, 0, 0, 
					0, 0, 1, 0, 
					0, 0, 0, 1);
	}

	const mat4 MatrixFactory::createScaleMat4(const Vec3& v) {
		return mat4(v.x, 0, 0, 0, 
					0, v.y, 0, 0,
					0, 0, v.z, 0,
					0, 0, 0,  1);
	}
	const mat4 MatrixFactory::createRotationMat4(const float angle, const Vec3& axis) {
		Vec3 normalized = normalize(axis);
		float c = cos(angle);
		float s = sin(angle);
		float ux = axis.x;
		float uy = axis.y;
		float uz = axis.z;
		return mat4(
			c + ux * ux * (1 - c), uy * ux * (1 - c) + uz * s, ux * uz * (1 - c) - uy * s, 0, 
			ux * uy * (1 - c) - uz * s, c + uy * uy * (1 - c), uz * uy * (1 - c) + ux * s, 0,
			ux * uz * (1 - c) + uy * s, uy * uz * (1 - c) - ux * s, c + uz * uz * (1 - c), 0,
			0, 0, 0, 1);
	}

	const mat4 MatrixFactory::createRotation2DMat4(const float angle) {
		float c = cos(angle);
		float s = sin(angle);
		return mat4(
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 1);
	}

	const mat4 MatrixFactory::createTranslationMat4(const Vec3& vector) {
		return mat4(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					vector.x, vector.y, vector.z, 1);
	}

	const mat4 MatrixFactory::createMat4FromMat3(const mat3& m3) {
		return mat4(m3.data[0], m3.data[1], m3.data[2], 0,
					m3.data[3], m3.data[4], m3.data[5], 0,
					m3.data[6], m3.data[7], m3.data[8], 0,
					0, 0, 0, 1
		);
	}



	const mat4 MatrixFactory::createViewMat4(const Vec3& eye, const Vec3& center, const Vec3& up) {
		Vec3 forward = normalize(center - eye);
		Vec3 right = normalize(cross(forward, up));
		Vec3 v = cross(right, forward);
		//forward = -forward;
		return mat4(right.x, v.x, -forward.x, 0,
					right.y, v.y, -forward.y, 0,
					right.z, v.z, -forward.z, 0,
					-dot(right, eye), -dot(v, eye), dot(forward, eye), 1
		);
	}


	const mat4 MatrixFactory::createOrtoMat4(const float left, const float right, const float bottom, const float top, const float near, const float far) {
		return mat4(2/ (right - left), 0, 0, 0,
			0, 2/ (top - bottom), 0, 0,
			0, 0, -2 / (far - near), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1
		);
	}


	const mat4 MatrixFactory::createPerspectiveMat4(const float fovy, const float aspect, const float nearZ, const float farZ) {
		return mat4( 1/ (aspect * tan(fovy/2 * M_PI / 180.0)), 0, 0, 0,
					0, 1 / tan(fovy / 2 * M_PI / 180.0), 0, 0,
					0, 0, -(farZ + nearZ) / (farZ - nearZ), -1,
					0, 0, -(2 * farZ * nearZ) / (farZ - nearZ), 0
		);
	}

	const mat4 MatrixFactory::createModelMatrix(Vec3 scale, float angle, Vec3 position, Vec3 parent_position, float parent_angle) {
		Vec3 position_relevant_parent = position + parent_position;
		float angle_relevant_parent = angle + parent_angle;

		return MatrixFactory::createTranslationMat4(parent_position) *
			MatrixFactory::createRotationMat4(parent_angle, Vec3(0, 1, 0)) *
			MatrixFactory::createTranslationMat4(position) *
			MatrixFactory::createRotationMat4(angle, Vec3(0, 1, 0)) *
			MatrixFactory::createScaleMat4(scale);

		//return 
			
			/*MatrixFactory::createTranslationMat4(position) *
			MatrixFactory::createRotationMat4(angle, Vec3(0, 1, 0)) *
			MatrixFactory::createTranslationMat4(parent_position)*
			MatrixFactory::createRotationMat4(parent_angle, Vec3(0, 1, 0)) *
			MatrixFactory::createScaleMat4(scale);*/
	}


	const mat4 MatrixFactory::createReflectionMatrix(Vec4 plane_vec) {
		float a = plane_vec.x;
		float b = plane_vec.y;
		float c = plane_vec.z;
		float d = plane_vec.w;

		return mat4(1 - 2 * a * a, -2 * a * b, -2 * a * c, 0,
			-2 * a * b, 1 - 2 * b * b, -2 * b * c, 0,
			-2 * a * c, -2 * b * c, 1 - 2 * c * c, 0,
			-2 * a * d, -2 * b * d, -2 * c * d, 1);
	}


	const mat4 MatrixFactory::createNEGY() {

		return mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1);
	}
}