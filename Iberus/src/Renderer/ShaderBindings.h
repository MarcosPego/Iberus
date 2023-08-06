#pragma once

namespace {
	class ShaderBindings {
		template<typename T>
		static void SetUniform(const GLchar* name, const T& vec);

	};
}



