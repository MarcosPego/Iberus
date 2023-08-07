#pragma once

#include "Mesh.h"
#include "Shader.h"
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
		ShaderRenderCmd(Shader* inboundShader) {
			shader = inboundShader;

			renderCmdType = RenderCmdType::PUSH_SHADER;
		}

		Shader* shader;
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

}


