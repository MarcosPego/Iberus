#include "Enginepch.h"
#include "OpenGLShaderBindings.h"

using namespace Math;

namespace Iberus {

	void ShaderBindings::SetUniformByLocation(GLuint programID, int location, int value) {
		if (location >= 0) {
			glUniform1i(location, value);
		}
	}
	void ShaderBindings::SetUniformByLocation(GLuint programID, int location, float value) {
		if (location >= 0) {
			glUniform1f(location, value);
		}
	}
	void ShaderBindings::SetUniformByLocation(GLuint programID, int location, const Vec2& value) {
		if (location >= 0) {
			glUniform2f(location, value.x, value.y);
		}
	}
	void ShaderBindings::SetUniformByLocation(GLuint programID, int location, const Vec3& value) {
		if (location >= 0) {
			glUniform3f(location, value.x, value.y, value.z);
		}
	}
	void ShaderBindings::SetUniformByLocation(GLuint programID, int location, const Vec4& value) {
		if (location >= 0) {
			glUniform4f(location, value.x, value.y, value.z, value.w);
		}
	}

}
