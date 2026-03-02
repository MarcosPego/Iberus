#pragma once

#include "MathUtils.h"

namespace Iberus {
	class ShaderBindings {
	public:
		static void BindAttribLocation(GLuint programID, int type, const GLchar* name);

		static int GetUniformLocation(GLuint programID, const GLchar* name);
		
		template<typename T>
		static void SetUniform(GLuint programID, const GLchar* name, const T& value);

		/// Set uniform by pre-resolved location (avoids per-frame glGetUniformLocation).
		static void SetUniformByLocation(GLuint programID, int location, int value);
		static void SetUniformByLocation(GLuint programID, int location, float value);
		static void SetUniformByLocation(GLuint programID, int location, const Math::Vec2& value);
		static void SetUniformByLocation(GLuint programID, int location, const Math::Vec3& value);
		static void SetUniformByLocation(GLuint programID, int location, const Math::Vec4& value);
	};
}



