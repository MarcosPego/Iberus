#include "Enginepch.h"
#include "OpenGLMesh.h"

// TODO(MPP) Global place to add these defines
#define VERTICES 0
#define UVS 1
#define NORMALS 2

namespace Iberus {
	bool OpenGLMesh::Build() {
		GLuint vboVertices, vboUVs, vboNormals;

		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		{
			glGenBuffers(1, &vboVertices);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3), &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(VERTICES);
			glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), 0);

			if (hasUVs)
			{
				glGenBuffers(1, &vboUVs);
				glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
				glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(Vec2), &uvs[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(UVS);
				glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), 0);
			}
			if (hasNormals)
			{
				glGenBuffers(1, &vboNormals);
				glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3), &normals[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(NORMALS);
				glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), 0);
			}
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vboVertices);
		glDeleteBuffers(1, &vboUVs);
		glDeleteBuffers(1, &vboNormals);

		return true; // TODO(MPP) This needs checks
	}

	void OpenGLMesh::Destroy() {
		glBindVertexArray(vaoID);
		glDisableVertexAttribArray(VERTICES);
		glDisableVertexAttribArray(UVS);
		glDisableVertexAttribArray(NORMALS);
		glDeleteVertexArrays(1, &vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void OpenGLMesh::Bind() {
		glBindVertexArray(vaoID);
	}

	void OpenGLMesh::Unbind() {
		glBindVertexArray(0);
	}
}