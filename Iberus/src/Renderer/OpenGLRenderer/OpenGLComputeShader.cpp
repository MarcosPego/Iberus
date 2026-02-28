#include "Enginepch.h"
#include "OpenGLComputeShader.h"
#include "Buffer.h"
#include <vector>

namespace Iberus {

	OpenGLComputeShader::OpenGLComputeShader(const std::string& inboundID, uint32_t inboundHandle, Buffer compBuffer)
		: ComputeShaderApi(inboundID, inboundHandle) {
		if (compBuffer.Invalid() || compBuffer.GetSize() == 0) {
			return;
		}
		std::string src(compBuffer.GetData(), compBuffer.GetData() + compBuffer.GetSize());
		const char* csrc = src.c_str();

		GLuint compShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compShader, 1, &csrc, nullptr);
		glCompileShader(compShader);
		GLint compiled = 0;
		glGetShaderiv(compShader, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE) {
			GLint len = 0;
			glGetShaderiv(compShader, GL_INFO_LOG_LENGTH, &len);
			if (len > 0) {
				std::vector<char> log(static_cast<size_t>(len) + 1, 0);
				glGetShaderInfoLog(compShader, len, nullptr, log.data());
				IB_CORE_ERROR("[ComputeShader] Compile failed for {}: {}", inboundID, log.data());
			}
			glDeleteShader(compShader);
			return;
		}

		programID = glCreateProgram();
		glAttachShader(programID, compShader);
		glLinkProgram(programID);
		glDetachShader(programID, compShader);
		glDeleteShader(compShader);

		GLint linked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE) {
			GLint len = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &len);
			if (len > 0) {
				std::vector<char> log(static_cast<size_t>(len) + 1, 0);
				glGetProgramInfoLog(programID, len, nullptr, log.data());
				IB_CORE_ERROR("[ComputeShader] Link failed for {}: {}", inboundID, log.data());
			}
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	OpenGLComputeShader::~OpenGLComputeShader() {
		if (programID != 0) {
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	void OpenGLComputeShader::Bind() const {
		glUseProgram(programID);
	}

	void OpenGLComputeShader::Unbind() const {
		glUseProgram(0);
	}

}
