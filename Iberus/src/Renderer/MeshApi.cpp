#include "Enginepch.h"
#include "MeshApi.h"


namespace Iberus {

	MeshApi::MeshApi(const std::string& inboundID, uint32_t inboundHandle, 
		const std::vector<Vec3>& vertices, const std::vector<Vec2>& uvs, const std::vector<Vec3>& normals) : RenderObject(inboundID, inboundHandle) {
		
	}

	MeshApi::~MeshApi() {
		Destroy();
	}
}