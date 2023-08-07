#pragma once

#include "ShaderApi.h"

#include "Buffer.h"

namespace Iberus {
	class OpenGLShader : public ShaderApi {
	public:
		OpenGLShader(const std::string& inboundID, uint32_t inboundHandle, Buffer vertexBuffer, Buffer fragBuffer) : ShaderApi(inboundID, inboundHandle) {
			Load(std::move(vertexBuffer), std::move(fragBuffer));
		}

		void Enable() const override;
		void Disable() const override;

		bool Load(Buffer vertexBuffer, Buffer fragBuffer) override;

		GLuint GetProgramID() const { return shaderGLID; }
	
	private:
		// Note(MPP) Not sure this should be private
		const GLuint AddShader(const GLenum shaderType, Buffer shaderBuffer);

		bool useNormals{ true };
		bool useUVs{ true };

		GLuint VertexShaderId{ 0 };
		GLuint FragmentShaderId{ 0 };
		GLuint shaderGLID{ 0 };
	};
}


