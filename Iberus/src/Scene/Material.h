#pragma once

namespace Iberus {

	class Shader;
	class Texture;

	class Material {
	public:	
		Material();
		~Material();

		/// Albedo

		/// Normal Map

		/// Metalic?

		void SetShader(Shader* inboundShader) {
			shader = inboundShader;
		}

		Shader* GetShader() const {
			return shader;
		}

	private:
		Shader* shader{ nullptr };

	};

}
