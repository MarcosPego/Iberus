#include "Enginepch.h"
#include "MaterialSerializer.h"
#include "Material.h"
#include "JsonValue.h"
#include "FileSystem.h"
#include "ResourceManager.h"
#include "IProvider.h"
#include "Shader.h"
#include "Texture.h"

#include <fstream>

namespace Iberus {

	using namespace Math;

	namespace {

		void SerializeVec4(JsonValue& obj, const char* key, const Vec4& v) {
			JsonValue arr = JsonValue::Array();
			arr.Set(0, JsonValue::Number(v.x));
			arr.Set(1, JsonValue::Number(v.y));
			arr.Set(2, JsonValue::Number(v.z));
			arr.Set(3, JsonValue::Number(v.w));
			obj.Set(key, arr);
		}

		Vec4 DeserializeVec4(const JsonValue& obj, const char* key) {
			JsonValue arr = obj[key];
			if (!arr.IsArray() || arr.Size() < 4) {
				return Vec4(1, 1, 1, 1);
			}
			return Vec4(arr[0].AsFloat(), arr[1].AsFloat(), arr[2].AsFloat(), arr[3].AsFloat());
		}

	}

	Buffer MaterialSerializer::Serialize(const Material& material) {
		JsonValue root = JsonValue::Object();
		root.Set("id", material.GetId());
		root.Set("shaderId", material.GetShaderId());
		SerializeVec4(root, "albedoColor", material.albedoColor);

		JsonValue texturesObj = JsonValue::Object();
		for (const auto& [slot, path] : material.GetTextureSlotPaths()) {
			texturesObj.Set(slot, path);
		}
		root.Set("textures", texturesObj);

		return root.ToBuffer(true);
	}

	std::unique_ptr<Material> MaterialSerializer::Deserialize(const Buffer& buffer, ResourceManager& resourceManager, IProvider* provider) {
		JsonValue root = JsonValue::Parse(buffer);
		if (!root.IsValid() || !root.IsObject()) {
			return nullptr;
		}

		std::string id = root.Contains("id") ? root["id"].AsString() : "Material";
		auto material = std::make_unique<Material>(id);

		material->albedoColor = DeserializeVec4(root, "albedoColor");

		if (root.Contains("shaderId")) {
			std::string shaderId = root["shaderId"].AsString();
			if (!shaderId.empty() && provider) {
				Shader* shader = resourceManager.GetOrCreateResource<Shader>(shaderId, provider);
				if (shader) {
					material->SetShader(shader);
				}
			}
		}

		if (root.Contains("textures") && root["textures"].IsObject()) {
			JsonValue texObj = root["textures"];
			std::vector<std::string> keys = texObj.Keys();
			for (const auto& slot : keys) {
				std::string path = texObj[slot].AsString();
				if (!path.empty() && provider) {
					Texture* texture = resourceManager.GetOrCreateResource<Texture>(path, provider);
					if (texture) {
						material->SetTexture(slot, texture);
					}
				}
			}
		}

		return material;
	}

	bool MaterialSerializer::SaveToFile(const Material& material, const std::string& path) {
		Buffer buf = Serialize(material);
		if (buf.Invalid()) {
			return false;
		}
		std::ofstream f(path, std::ios::binary);
		if (!f) {
			return false;
		}
		f.write(reinterpret_cast<const char*>(buf.GetData()), static_cast<std::streamsize>(buf.GetSize()));
		return f.good();
	}

	std::unique_ptr<Material> MaterialSerializer::LoadFromFile(const std::string& path, ResourceManager& resourceManager, IProvider* provider) {
		Buffer buf = FileSystem::GetRawFileBuffer(path);
		if (buf.Invalid()) {
			return nullptr;
		}
		return Deserialize(buf, resourceManager, provider);
	}

}
