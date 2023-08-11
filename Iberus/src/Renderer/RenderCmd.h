#pragma once

#include "MathUtils.h"

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
		PUSH_TEXTURE,
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
			ID = inboundShaderID;

			renderCmdType = RenderCmdType::PUSH_SHADER;
		}

		std::string ID;
	};

	class MeshRenderCmd : public RenderCmd {
	public:
		MeshRenderCmd(const std::string& inboundMeshID) {
			ID = inboundMeshID;

			renderCmdType = RenderCmdType::PUSH_MESH;
		}

		std::string ID;
	};

	class TextureRenderCmd : public RenderCmd {
	public:
		TextureRenderCmd(const std::string& inboundID, const std::string& inTargetTexture, int targetBind) {
			targetTexture = inTargetTexture;
			ID = inboundID;
			bind = targetBind;

			renderCmdType = RenderCmdType::PUSH_TEXTURE;
		}

		std::string targetTexture;
		std::string ID;
		int bind{ 0 };
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
		UploadMeshRenderCmd(const std::string& inboundID, const std::vector<Vec3>& inboundVertices, const std::vector<Vec2>& inboundUvs, const std::vector<Vec3>& inboundNormals) {
			ID = inboundID;
			vertices = inboundVertices;
			uvs = inboundUvs;
			normals = inboundNormals;

			renderCmdType = RenderCmdType::UPLOAD_MESH;
		}

		std::string ID;
		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
	};

	class DeleteMeshRenderCmd : public RenderCmd {
	public:
		DeleteMeshRenderCmd(const std::string& inboundID) {
			ID = inboundID;

			renderCmdType = RenderCmdType::DELETE_MESH;
		}

		std::string ID;
	};

	class UploadShaderRenderCmd : public RenderCmd {
	public:
		UploadShaderRenderCmd(const std::string& inboundID, Buffer inboundVertexBuffer, Buffer inboundFragBuffer) {
			ID = inboundID;
			vertexBuffer = std::move(inboundVertexBuffer);
			fragBuffer = std::move(inboundFragBuffer);

			renderCmdType = RenderCmdType::UPLOAD_SHADER;
		}

		std::string ID;
		Buffer vertexBuffer;
		Buffer fragBuffer;
	};

	class DeleteShaderRenderCmd : public RenderCmd {
	public:
		DeleteShaderRenderCmd(const std::string& inboundID) {
			ID = inboundID;

			renderCmdType = RenderCmdType::DELETE_SHADER;
		}

		std::string ID;
	};

	class UploadTextureRenderCmd : public RenderCmd {
	public:
		UploadTextureRenderCmd(const std::string& inboundID, Buffer inboundBuffer, int inWidth, int inHeight, int inChannels) {
			ID = inboundID;
			width = inWidth;
			height = inHeight;
			channels = inChannels;
			buffer = std::move(inboundBuffer);

			renderCmdType = RenderCmdType::UPLOAD_TEXTURE;
		}

		std::string ID;
		Buffer buffer;
		int width, height, channels;
	};

	class DeleteTextureRenderCmd : public RenderCmd {
	public:
		DeleteTextureRenderCmd(const std::string& inboundID) {
			ID = inboundID;

			renderCmdType = RenderCmdType::DELETE_TEXTURE;
		}

		std::string ID;
	};

}


