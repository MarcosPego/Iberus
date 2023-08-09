#pragma once

#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	class RenderBatch;
	class Shader;
	class Texture;

	class IBERUS_API Material {
	public:	
		explicit Material(const std::string& inID);
		virtual ~Material() = default;

		/// Albedo
		Vec4 albedoColor{0.5f, 1.0f, 0.5f, 1.0f};

		/// Normal Map

		/// Metalic?

		void SetShader(Shader* inboundShader) {
			shader = inboundShader;
		}

		Shader* GetShader() const {
			return shader;
		}

		void PushDraw(RenderBatch& renderBatch);

	private:
		std::string ID;

		Shader* shader{ nullptr };

	};

}
