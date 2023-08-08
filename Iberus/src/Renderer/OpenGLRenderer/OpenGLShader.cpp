#include "Enginepch.h"
#include "OpenGLShader.h"

#include "ShaderBindings.h"

#define VERTICES 0
#define UVS 1
#define NORMALS 2

namespace Iberus {

void OpenGLShader::Enable() const {
	glUseProgram(shaderGLID);
}

void OpenGLShader::Disable() const {
	glUseProgram(0);
}

bool OpenGLShader::Load(Buffer vertexBuffer, Buffer fragBuffer) {
	shaderGLID = glCreateProgram();

	Log::GetClientLogger()->info("HERE!");

	GLuint VertexShaderId = AddShader(GL_VERTEX_SHADER, std::move(vertexBuffer));
	GLuint FragmentShaderId = AddShader(GL_FRAGMENT_SHADER, std::move(fragBuffer));

	const std::string positionName = "inVertices";
	const std::string uvName = "inUVs";
	const std::string normalName = "inNormals";

	/// Bind Attributes
	ShaderBindings::BindAttribLocation(shaderGLID, VERTICES, positionName.c_str());
	if (useUVs) {
		ShaderBindings::BindAttribLocation(shaderGLID, UVS, uvName.c_str());
	} 
	if (useNormals) {
		ShaderBindings::BindAttribLocation(shaderGLID, NORMALS, normalName.c_str());
	}

	// Link Shader
	glLinkProgram(shaderGLID);
	GLint linked;
	glGetProgramiv(shaderGLID, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		GLint length;
		glGetProgramiv(shaderGLID, GL_INFO_LOG_LENGTH, &length);
		GLchar* const log = new char[length];
		glGetProgramInfoLog(shaderGLID, length, &length, log);
		std::cerr << "[LINK] " << std::endl << log << std::endl;
		delete[] log;

		return false;
	}

	glDetachShader(shaderGLID, VertexShaderId);
	glDetachShader(shaderGLID, FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	//modelMatrix_UId = glGetUniformLocation(shaderGLID, "ModelMatrix");
	//viewMatrix_UId = glGetUniformLocation(shaderGLID, "ViewMatrix");
	//projectionMatrix_UId = glGetUniformLocation(shaderGLID, "ProjectionMatrix");

	return true;
}

const GLuint OpenGLShader::AddShader(const GLenum shaderType, Buffer shaderBuffer){
	const GLuint shaderID = glCreateShader(shaderType);
	const std::string shaderCode(shaderBuffer.data.get(), shaderBuffer.data.get() + shaderBuffer.size);
	const GLchar* code = shaderCode.c_str();

	glShaderSource(shaderID, 1, &code, 0);
	glCompileShader(shaderID);

	//checkCompilation(shader_id, filename); // TODO Check compilation

	GLint compiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		GLchar* const log = new char[length];
		glGetShaderInfoLog(shaderID, length, &length, log);
		std::cerr << "[" << "shaderFailed" << "] " << std::endl << log;
		delete[] log;
	}

	glAttachShader(shaderGLID, shaderID);
	return shaderID;
}

}

