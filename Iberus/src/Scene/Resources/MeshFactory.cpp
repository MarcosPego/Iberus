#include "Enginepch.h"
#include "MeshFactory.h"

#include "MathUtils.h"
using namespace Math;

namespace Iberus {

	Mesh* MeshFactory::CreateQuad(const std::string& ID, ResourceManager& resourceManager, int width, int height) {
        auto* mesh = resourceManager.GetResource<Mesh>(ID);
        if (mesh) {
            return mesh;
        }

        float x, y, w, h;

        x = 0;
        y = 0;
        w = width;
        h = height;

        std::vector<Vec3> vertices = {
            { x , y, 0.0 },
            { x , h, 0.0 },
            { w , h, 0.0 },

            { w , h, 0.0 },
            { w , y, 0.0 },
            { x , y, 0.0 }
        };

        std::vector<Vec3> normals = {
            { 0.0f, 0.0f,1.0f },
            { 0.0f, 0.0f,1.0f },
            { 0.0f, 0.0f,1.0f },

            { 0.0f, 0.0f,1.0f },
            { 0.0f, 0.0f,1.0f },
            { 0.0f, 0.0f,1.0f }
        };
        std::vector<Vec2> uvs = {
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },

            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
            { 0.0f, 0.0f }
        };

        return resourceManager.CreateResource<Mesh>(ID, vertices, uvs, normals);
	}

}