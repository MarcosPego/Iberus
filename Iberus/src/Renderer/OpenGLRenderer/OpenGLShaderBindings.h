#pragma once

#include "ShaderBindings.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {

	void ShaderBindings::BindAttribLocation(GLuint programID, int type, const GLchar* name) {
		return glBindAttribLocation(programID, type, name);
	}

	int ShaderBindings::GetUniformLocation(GLuint programID, const GLchar* name) {
		return glGetUniformLocation(programID, name);
	}

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const float& value) {
		glUniform1f(GetUniformLocation(programID, name), value);
	}

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const int& value) {
		glUniform1i(GetUniformLocation(programID, name), value);
	}

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const Vec2& value) {
		glUniform2f(GetUniformLocation(programID, name), value.x, value.y);
	}

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const Vec3& value) {
		glUniform3f(GetUniformLocation(programID, name), value.x, value.y, value.z);
	}

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const Vec4& value) {
		glUniform4f(GetUniformLocation(programID, name), value.x, value.y, value.z, value.w);
	}

	// TODO Color Uniform!
	/*template<>
	static void SetUniform(GLuint programID, const GLchar* name, const Vec4& value) {
		glUniform4fv(GetUniformLocation(programID, name), 1, value.data);
	}*/

	template<>
	static void ShaderBindings::SetUniform(GLuint programID, const GLchar* name, const Mat4& value) {
		glUniformMatrix4fv(GetUniformLocation(programID, name), 1, GL_FALSE, value.data);
	}

/*class OpenGLShaderBindings : public ShaderBindings {
public:

};*/

}

