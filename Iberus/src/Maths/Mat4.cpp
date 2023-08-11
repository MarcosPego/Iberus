#include "Enginepch.h"
#include "Matrix.h"


namespace Math {

	Mat4::Mat4() : data{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } {
	}

	Mat4::Mat4(const float k) : data{ k,k,k,k,k,k,k,k,k,k,k,k,k,k,k,k } {
	}

	Mat4::Mat4(const float m0, const float m1, const float m2, const float m3,
		const float m4, const float m5, const float m6, const float m7,
		const float m8, const float m9, const float m10, const float m11,
		const float m12, const float m13, const float m14, const float m15)
		: data{ m0 , m1 , m2, m3, m4 , m5 , m6, m7, m8, m9 , m10 , m11 , m12, m13, m14 , m15 } {
	}

	//Column major constructor
	Mat4::Mat4(const Mat4& m) : data{
			m.data[0] , m.data[1] , m.data[2] , m.data[3], 
			m.data[4] , m.data[5] , m.data[6] , m.data[7], 
			m.data[8] , m.data[9] , m.data[10] , m.data[11],
			m.data[12] , m.data[13] , m.data[14] , m.data[15]
	} {
	}

	Mat4::Mat4(const Mat3& m) : data{
			m.data[0] , m.data[1] , m.data[2] , 0,
			m.data[3] , m.data[4] , m.data[5] , 0,
			m.data[6] , m.data[7] , m.data[8] , 0,
			0 , 0 , 0 ,1
	} {
	}

	void Mat4::clean() {
		for (int i = 0; i < this->array_size; i++) {
			if (fabs(this->data[i]) < EPSILON) this->data[i] = 0.0f;
		}
	}

	Mat4& Mat4::operator - (const Mat4& m) {
		for (int i = 0; i < m.array_size; i++) {
			this->data[i] = -m.data[i];
		}
		return(*this);
	}

	Mat4 transpose(const Mat4& m) {
		Mat4 new_matrix = Mat4();
		for (int line = 0; line < 4; line++) {
			for (int col = 0; col < 4; col++) {
				new_matrix.data[line + col * 4] = m.data[line * 4 + col];
			}
		}
		return new_matrix;
	}

	Mat4 inverse(const Mat4& m)
	{
		Mat4 new_matrix = Mat4();

		new_matrix.data[0] = m.data[5] * m.data[10] * m.data[15] - m.data[5] * m.data[11] * m.data[14] - m.data[9] * m.data[6] * m.data[15] + m.data[9] * m.data[7] * m.data[14] + m.data[13] * m.data[6] * m.data[11] - m.data[13] * m.data[7] * m.data[10];
		new_matrix.data[4] = -m.data[4] * m.data[10] * m.data[15] + m.data[4] * m.data[11] * m.data[14] + m.data[8] * m.data[6] * m.data[15] - m.data[8] * m.data[7] * m.data[14] - m.data[12] * m.data[6] * m.data[11] + m.data[12] * m.data[7] * m.data[10];
		new_matrix.data[8] = m.data[4] * m.data[9] * m.data[15] - m.data[4] * m.data[11] * m.data[13] - m.data[8] * m.data[5] * m.data[15] + m.data[8] * m.data[7] * m.data[13] + m.data[12] * m.data[5] * m.data[11] - m.data[12] * m.data[7] * m.data[9];
		new_matrix.data[12] = -m.data[4] * m.data[9] * m.data[14] + m.data[4] * m.data[10] * m.data[13] + m.data[8] * m.data[5] * m.data[14] - m.data[8] * m.data[6] * m.data[13] - m.data[12] * m.data[5] * m.data[10] + m.data[12] * m.data[6] * m.data[9];
		new_matrix.data[1] = -m.data[1] * m.data[10] * m.data[15] + m.data[1] * m.data[11] * m.data[14] + m.data[9] * m.data[2] * m.data[15] - m.data[9] * m.data[3] * m.data[14] - m.data[13] * m.data[2] * m.data[11] + m.data[13] * m.data[3] * m.data[10];
		new_matrix.data[5] = m.data[0] * m.data[10] * m.data[15] - m.data[0] * m.data[11] * m.data[14] - m.data[8] * m.data[2] * m.data[15] + m.data[8] * m.data[3] * m.data[14] + m.data[12] * m.data[2] * m.data[11] - m.data[12] * m.data[3] * m.data[10];
		new_matrix.data[9] = -m.data[0] * m.data[9] * m.data[15] + m.data[0] * m.data[11] * m.data[13] + m.data[8] * m.data[1] * m.data[15] - m.data[8] * m.data[3] * m.data[13] - m.data[12] * m.data[1] * m.data[11] + m.data[12] * m.data[3] * m.data[9];
		new_matrix.data[13] = m.data[0] * m.data[9] * m.data[14] - m.data[0] * m.data[10] * m.data[13] - m.data[8] * m.data[1] * m.data[14] + m.data[8] * m.data[2] * m.data[13] + m.data[12] * m.data[1] * m.data[10] - m.data[12] * m.data[2] * m.data[9];
		new_matrix.data[2] = m.data[1] * m.data[6] * m.data[15] - m.data[1] * m.data[7] * m.data[14] - m.data[5] * m.data[2] * m.data[15] + m.data[5] * m.data[3] * m.data[14] + m.data[13] * m.data[2] * m.data[7] - m.data[13] * m.data[3] * m.data[6];
		new_matrix.data[6] = -m.data[0] * m.data[6] * m.data[15] + m.data[0] * m.data[7] * m.data[14] + m.data[4] * m.data[2] * m.data[15] - m.data[4] * m.data[3] * m.data[14] - m.data[12] * m.data[2] * m.data[7] + m.data[12] * m.data[3] * m.data[6];
		new_matrix.data[10] = m.data[0] * m.data[5] * m.data[15] - m.data[0] * m.data[7] * m.data[13] - m.data[4] * m.data[1] * m.data[15] + m.data[4] * m.data[3] * m.data[13] + m.data[12] * m.data[1] * m.data[7] - m.data[12] * m.data[3] * m.data[5];
		new_matrix.data[14] = -m.data[0] * m.data[5] * m.data[14] + m.data[0] * m.data[6] * m.data[13] + m.data[4] * m.data[1] * m.data[14] - m.data[4] * m.data[2] * m.data[13] - m.data[12] * m.data[1] * m.data[6] + m.data[12] * m.data[2] * m.data[5];
		new_matrix.data[3] = -m.data[1] * m.data[6] * m.data[11] + m.data[1] * m.data[7] * m.data[10] + m.data[5] * m.data[2] * m.data[11] - m.data[5] * m.data[3] * m.data[10] - m.data[9] * m.data[2] * m.data[7] + m.data[9] * m.data[3] * m.data[6];
		new_matrix.data[7] = m.data[0] * m.data[6] * m.data[11] - m.data[0] * m.data[7] * m.data[10] - m.data[4] * m.data[2] * m.data[11] + m.data[4] * m.data[3] * m.data[10] + m.data[8] * m.data[2] * m.data[7] - m.data[8] * m.data[3] * m.data[6];
		new_matrix.data[11] = -m.data[0] * m.data[5] * m.data[11] + m.data[0] * m.data[7] * m.data[9] + m.data[4] * m.data[1] * m.data[11] - m.data[4] * m.data[3] * m.data[9] - m.data[8] * m.data[1] * m.data[7] + m.data[8] * m.data[3] * m.data[5];
		new_matrix.data[15] = m.data[0] * m.data[5] * m.data[10] - m.data[0] * m.data[6] * m.data[9] - m.data[4] * m.data[1] * m.data[10] + m.data[4] * m.data[2] * m.data[9] + m.data[8] * m.data[1] * m.data[6] - m.data[8] * m.data[2] * m.data[5];

		float det = m.data[0] * new_matrix.data[0] + m.data[1] * new_matrix.data[4] + m.data[2] * new_matrix.data[8] + m.data[3] * new_matrix.data[12];

		if (det == 0)
			return false;

		det = 1.f / det;

		Mat4 final_matrix = Mat4();
		for (int i = 0; i < 16; i++)
			final_matrix.data[i] = new_matrix.data[i] * det;

		return final_matrix;
	}



	Mat4& Mat4::operator+= (const Mat4& m) {
		for (int i = 0; i < m.array_size; i++) {
			this->data[i] += m.data[i];
		}
		return(*this);
	};

	Mat4& Mat4::operator-= (const Mat4& m) {
		for (int i = 0; i < m.array_size; i++) {
			this->data[i] -= m.data[i];
		}
		return(*this);
	};

	Mat4& Mat4::operator*= (const Mat4& m) {
		Mat4 new_matrix = Mat4();
		new_matrix += (*this);
		//line
		for (int line = 0; line < 4; line++) {
			//col
			for (int col = 0; col < 4; col++) {
				//this->data[line + col * 3] = this->data[col] * m.data[line * 3] + this->data[col+3] * m.data[line * 3 + 1] + this->data[col + 6] * m.data[line * 3 + 2];
				new_matrix.data[line + col * 4] = this->data[line] * m.data[col * 4] + this->data[line + 4] * m.data[col * 4 + 1] + this->data[line + 8] * m.data[col * 4 + 2] + this->data[line + 12] * m.data[col * 4 + 3];
			}
		}
		for (int i = 0; i < new_matrix.array_size; i++) {
			this->data[i] = new_matrix.data[i];
		}

		return(*this);
	};


	const Mat4 operator + (const Mat4& m0, const Mat4& m1) {

		Mat4 new_matrix = Mat4();
		new_matrix += m0;
		new_matrix += m1;

		return new_matrix;
	};

	const Mat4 operator - (const Mat4& m0, const Mat4& m1) {
		Mat4 new_matrix = Mat4();
		new_matrix += m0;
		new_matrix -= m1;

		return new_matrix;
	};

	const Mat4 operator * (const Mat4& m0, const Mat4& m1) {
		Mat4 new_matrix = Mat4();
		new_matrix += m0;
		new_matrix *= m1;

		return new_matrix;
	};

	const Mat4 operator * (const Mat4& m, const float k) {
		Mat4 new_matrix = Mat4();
		for (int i = 0; i < m.array_size; i++) {
			new_matrix.data[i] = m.data[i] * k;
		}
		return new_matrix;
	};

	const Mat4 operator * (const float k, const Mat4& m) {
		Mat4 new_matrix = Mat4();
		for (int i = 0; i < m.array_size; i++) {
			new_matrix.data[i] = m.data[i] * k;
		}
		return new_matrix;
	};

	const Vec4 operator * (const Mat4& m, const Vec4& v) {
		Vec4 new_vector = Vec4();
		new_vector.x = v.x * m.data[0] + v.y * m.data[4] + v.z * m.data[8] + v.z * m.data[12];
		new_vector.y = v.x * m.data[1] + v.y * m.data[5] + v.z * m.data[9] + v.z * m.data[13];
		new_vector.z = v.x * m.data[2] + v.y * m.data[6] + v.z * m.data[10] + v.z * m.data[14];
		new_vector.w = v.x * m.data[3] + v.y * m.data[7] + v.z * m.data[11] + v.z * m.data[15];

		return new_vector;
	};


	const bool operator == (const Mat4& m0, const Mat4& m1) {
		for (int i = 0; i < m0.array_size; i++) {
			if (fabs(m0.data[i] - m1.data[i]) > EPSILON)
			{
				return false;
			}
		}
		return true;
	}

	const bool operator != (const Mat4& m0, const Mat4& m1) {
		for (int i = 0; i < m0.array_size; i++) {
			if (fabs(m0.data[i] - m1.data[i]) > EPSILON)
			{
				return true;
			}
		}
		return false;
	}

	std::ostream& operator << (std::ostream& os, const Mat4& m) {
		os << "{ " << m.data[0] << ", " << m.data[4] << ", " << m.data[8] << ", " << m.data[12]
			<< "\n  \0" << m.data[1] << ", " << m.data[5] << ", " << m.data[9] << ", " << m.data[13]
			<< "\n  \0" << m.data[2] << ", " << m.data[6] << ", " << m.data[10] << ", " << m.data[14]
			<< "\n  \0" << m.data[3] << ", " << m.data[7] << ", " << m.data[11] << ", " << m.data[15]  << " }";
		return os;
	}

	std::istream& operator >> (std::istream& is, Mat4& m) {
		for (int i = 0; i < m.array_size; i++) {
			is >> m.data[i];
		}
		return is;
	};
}