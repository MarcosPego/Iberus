#pragma once

#include "ComputeShaderApi.h"
#include "Buffer.h"

namespace Iberus {

	class OpenGLComputeShader : public ComputeShaderApi {
	public:
		OpenGLComputeShader(const std::string& inboundID, uint32_t inboundHandle, Buffer compBuffer);
		~OpenGLComputeShader();

		void Bind() const override;
		void Unbind() const override;
		unsigned int GetProgramID() const override { return programID; }

	private:
		unsigned int programID{ 0 };
	};

}
