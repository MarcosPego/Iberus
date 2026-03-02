#pragma once

#include "MathUtils.h"
#include "Resource.h"

#include <string>
#include <unordered_map>

using namespace Math;

namespace Iberus {
	static const std::string ALBEDOTEXTURE{ "albedoTexture" };
	static const std::string NORMALTEXTURE{ "normalTexture" };
	static const std::string METALICTEXTURE{ "metallicTexture" };

	class RenderBatch;
	class Shader;
	class Texture;

	class IBERUS_API Material : public Resource {
	public:
		explicit Material(const std::string& inID);
		virtual ~Material() = default;

		/// Albedo
		Vec4 albedoColor{1.0f, 1.0f, 1.0f, 1.0f};

		/// Emissive (glow)
		Vec3 emissiveColor{0.0f, 0.0f, 0.0f};
		float emissiveIntensity{0.0f};

		/// Normal Map

		/// Metalic?

		void SetShader(Shader* inboundShader);

		Shader* GetShader() const {
			return shader;
		}


		void SetTexture(const std::string& ID, Texture* inTexture);

		void PushDraw(RenderBatch& renderBatch);

		const std::string& GetId() const { return GetID(); }
		std::string GetShaderId() const;
		std::unordered_map<std::string, std::string> GetTextureSlotPaths() const;

	private:
		void BindTextures(RenderBatch& renderBatch);

		std::unordered_map<std::string, int> texturesBindings;
		std::unordered_map<std::string, Texture*> textures;
		Shader* shader{ nullptr };

	};

}
