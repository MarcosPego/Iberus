#include "Enginepch.h"
#include "OpenGLShader.h"

#define VERTICES 0
#define TEXCOORDS 1
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

	GLuint VertexShaderId = AddShader(GL_VERTEX_SHADER, std::move(vertexBuffer));
	GLuint FragmentShaderId = AddShader(GL_FRAGMENT_SHADER, std::move(fragBuffer));
	
	/// Bindings here!
	/*glBindAttribLocation(shaderGLID, VERTICES, "in_Position");
	if (TexcoordsLoaded)
		glBindAttribLocation(shader_id, TEXCOORDS, "inTexcoord");
	if (NormalsLoaded)
		glBindAttribLocation(shader_id, NORMALS, "inNormal");

	glLinkProgram(shader_id);*/

	// Link Shader
	GLint linked;
	glGetProgramiv(shaderGLID, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
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

	//modelMatrix_UId = glGetUniformLocation(shader_id, "ModelMatrix");
	//viewMatrix_UId = glGetUniformLocation(shader_id, "ViewMatrix");
	//projectionMatrix_UId = glGetUniformLocation(shader_id, "ProjectionMatrix");

	return true;
}

const GLuint OpenGLShader::AddShader(const GLenum shaderType, Buffer shaderBuffer){
	const GLuint shaderID = glCreateShader(shaderType);
	const std::string shaderCode(shaderBuffer.data.get(), shaderBuffer.data.get() + shaderBuffer.size);
	const GLchar* code = shaderCode.c_str();

	glShaderSource(shaderID, 1, &code, 0);
	glCompileShader(shaderID);
	//checkCompilation(shader_id, filename); // TODO Check compilation
	glAttachShader(shaderGLID, shaderID);
	return shaderID;
}

}

