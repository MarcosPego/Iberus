#pragma once

#include "Shader.h"

namespace Iberus {
	class OpenGLShader : public Shader {
	public:
		void Enable() const override;
		void Disable() const override;

		bool Load(Buffer vertexBuffer, Buffer fragBuffer) override;
	
	private:
		// Note(MPP) Not sure this should be private
		const GLuint AddShader(const GLenum shaderType, Buffer shaderBuffer);


		GLuint VertexShaderId;
		GLuint FragmentShaderId;
		GLuint shaderGLID;
	};
}


