#pragma once

#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	static const std::string ALBEDOTEXTURE{ "albedoTexture" };
	static const std::string NORMALTEXTURE{ "normalTexture" };
	static const std::string METALICTEXTURE{ "metallicTexture" };

	class RenderBatch;
	class Shader;
	class Texture;

	class IBERUS_API Material {
	public:	
		explicit Material(const std::string& inID);
		virtual ~Material() = default;

		/// Albedo
		Vec4 albedoColor{1.0f, 1.0f, 1.0f, 1.0f};

		/// Normal Map

		/// Metalic?

		void SetShader(Shader* inboundShader);

		Shader* GetShader() const {
			return shader;
		}

		void BindTextures();
		void SetTexture(const std::string& ID, Texture* inTexture);

		void PushDraw(RenderBatch& renderBatch);

	private:
		std::string ID;

		std::unordered_map<std::string, int> texturesBindings;
		std::unordered_map<std::string, Texture*> textures;
		Shader* shader{ nullptr };

	};

}
