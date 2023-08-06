#pragma once

namespace Iberus {
	class ShaderBindings {
	public:
		static void BindAttribLocation(GLuint programID, int type, const GLchar* name);

		static int GetUniformLocation(GLuint programID, const GLchar* name);
		
		template<typename T>
		static void SetUniform(GLuint programID, const GLchar* name, const T& value);

	};
}



