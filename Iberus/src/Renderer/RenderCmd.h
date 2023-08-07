#pragma once

#include "Mesh.h"
#include "ShaderApi.h"
#include "MathUtils.h"
#include "Buffer.h"

using namespace Math;

namespace Iberus {
	enum class UniformType {
		NULLTYPE,
		INT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT4
	};

	enum class RenderCmdType {
		NULLCMD = 0,
		UPLOAD_SHADER,
		UPLOAD_MESH,
		UPLOAD_TEXTURE,
		PUSH_MESH,
		PUSH_UNIFORM,
		PUSH_SHADER,
		PUSH_CAMERA,
		DELETE_SHADER,
		DELETE_MESH,
		DELETE_TEXTURE,
	};

	class RenderCmd {
	public:
		virtual ~RenderCmd() = default;

		void SetRenderCmdType(RenderCmdType newRenderCmdType) {
			renderCmdType = newRenderCmdType;
		}

		RenderCmdType GetRenderCmdType() const {
			return renderCmdType;
		}

		UniformType GetUniformType() const {
			return uniType;
		}

	protected:
		UniformType uniType{ UniformType::NULLTYPE };
		RenderCmdType renderCmdType{ RenderCmdType::NULLCMD };
	};

	class ShaderRenderCmd : public RenderCmd {
	public:
		ShaderRenderCmd(const std::string& inboundShaderID) {
			shaderID = inboundShaderID;

			renderCmdType = RenderCmdType::PUSH_SHADER;
		}

		std::string shaderID;
	};

	class MeshRenderCmd : public RenderCmd {
	public:
		MeshRenderCmd(const std::string& inboundMeshID) {
			meshID = inboundMeshID;

			renderCmdType = RenderCmdType::PUSH_MESH;
		}

		std::string meshID;
	};

	template<typename T>
	class UniformRenderCmd : public RenderCmd {
	public:
		UniformRenderCmd(std::string inboundName, const T& inboundValue, UniformType inboundUniType) {
			name = inboundName;
			value = inboundValue;
			uniType = inboundUniType;

			renderCmdType = RenderCmdType::PUSH_UNIFORM;
		}

		const std::string& GetName() const {
			return name;
		}

		T GetValue() const {
			return value;
		}

	private:
		std::string name;
		T value;
	};

	class CameraRenderCmd : public RenderCmd {
	public:
		CameraRenderCmd(Mat4 inboundViewMatrix, Mat4 inboundProjectionMatrix) {
			projectionMatrix = inboundProjectionMatrix;
			viewMatrix = inboundViewMatrix;
			renderCmdType = RenderCmdType::PUSH_CAMERA;
		}

		Mat4 projectionMatrix;
		Mat4 viewMatrix;
	};

	/// Create and Delete Cmds

	class UploadMeshRenderCmd : public RenderCmd {
	public:
		UploadMeshRenderCmd(const std::string& inboundMeshID, const std::vector<Vec3>& inboundVertices, const std::vector<Vec2>& inboundUvs, const std::vector<Vec3>& inboundNormals) {
			meshID = inboundMeshID;
			vertices = inboundVertices;
			uvs = inboundUvs;
			normals = inboundNormals;

			renderCmdType = RenderCmdType::UPLOAD_MESH;
		}

		std::string meshID;
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
	};

	class DeleteMeshRenderCmd : public RenderCmd {
	public:
		DeleteMeshRenderCmd(const std::string& inboundMeshID) {
			meshID = inboundMeshID;

			renderCmdType = RenderCmdType::DELETE_MESH;
		}

		std::string meshID;
	};

	class UploadShaderRenderCmd : public RenderCmd {
	public:
		UploadShaderRenderCmd(const std::string& inboundShaderID, Buffer inboundVertexBuffer, Buffer inboundFragBuffer) {
			shaderID = inboundShaderID;
			vertexBuffer = std::move(inboundVertexBuffer);
			fragBuffer = std::move(inboundFragBuffer);

			renderCmdType = RenderCmdType::UPLOAD_SHADER;
		}

		std::string shaderID;
		Buffer vertexBuffer;
		Buffer fragBuffer;
	};

	class DeleteShaderRenderCmd : public RenderCmd {
	public:
		DeleteShaderRenderCmd(const std::string& inboundShaderID) {
			shaderID = inboundShaderID;

			renderCmdType = RenderCmdType::DELETE_SHADER;
		}

		std::string shaderID;
	};

}


