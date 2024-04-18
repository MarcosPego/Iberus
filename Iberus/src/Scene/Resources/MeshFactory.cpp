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

    // Calculate vertex positions for the current grid cell
/*float xPos0 = x * stepX - halfWidth;
float yPos0 = y * stepY - halfHeight;
float xPos1 = (x + 1) * stepX - halfWidth;
float yPos1 = y * stepY - halfHeight;
float xPos2 = (x + 1) * stepX - halfWidth;
float yPos2 = (y + 1) * stepY - halfHeight;
float xPos3 = x * stepX - halfWidth;
float yPos3 = (y + 1) * stepY - halfHeight;
vertices.push_back(Vec3(xPos0, yPos0, 0.0f));
vertices.push_back(Vec3(xPos1, yPos1, 0.0f));
vertices.push_back(Vec3(xPos2, yPos2, 0.0f));

vertices.push_back(Vec3(xPos2, yPos2, 0.0f));
vertices.push_back(Vec3(xPos3, yPos3, 0.0f));
vertices.push_back(Vec3(xPos0, yPos0, 0.0f));*/



    Mesh* MeshFactory::CreatePlane(const std::string& ID, ResourceManager& resourceManager, int width, int height, std::vector<float> heightMap) {
        auto* mesh = resourceManager.GetResource<Mesh>(ID);
        if (mesh) {
            return mesh;
        }
        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Vec2> uvs;

        float halfWidth = static_cast<float>(width) / 2.0f;
        float halfHeight = static_cast<float>(height) / 2.0f;

        float stepX = width > 1 ? 1.0f / (width - 1) : 0.0f;
        float stepY = height > 1 ? 1.0f / (height - 1) : 0.0f;

        for (int y = 0; y < height - 1; y++) {
            for (int x = 0; x < width - 1; x++) {
                float xPos0 = x * stepX - halfWidth;
                float yPos0 = heightMap[y * width + x] * 1;
                float zPos0 = y * stepY - halfHeight;

                float xPos1 = (x + 1) * stepX - halfWidth;
                float yPos1 = heightMap[y * width + (x + 1)] * 1;
                float zPos1 = y * stepY - halfHeight;

                float xPos2 = (x + 1) * stepX - halfWidth;
                float yPos2 = heightMap[(y + 1) * width + (x + 1)] * 1;
                float zPos2 = (y + 1) * stepY - halfHeight;

                float xPos3 = x * stepX - halfWidth;
                float yPos3 = heightMap[(y + 1) * width + x] * 1;
                float zPos3 = (y + 1) * stepY - halfHeight;

                // Create vertices for the two triangles in the grid cell
                vertices.push_back(Vec3(xPos0, yPos0, zPos0));
                vertices.push_back(Vec3(xPos1, yPos1, zPos1));
                vertices.push_back(Vec3(xPos2, yPos2, zPos2));

                vertices.push_back(Vec3(xPos2, yPos2, zPos2));
                vertices.push_back(Vec3(xPos3, yPos3, zPos3));
                vertices.push_back(Vec3(xPos0, yPos0, zPos0));


                // Calculate normals (assuming upward-facing normals for a plane)
                for (int i = 0; i < 6; i++) {
                    normals.push_back(Vec3(0.0f, 0.0f, 1.0f));
                }

                // Calculate UV coordinates
                float u0 = static_cast<float>(x) / (width - 1);
                float u1 = static_cast<float>(x + 1) / (width - 1);
                float v0 = static_cast<float>(y) / (height - 1);
                float v1 = static_cast<float>(y + 1) / (height - 1);

                uvs.push_back(Vec2(u0, v0));
                uvs.push_back(Vec2(u1, v0));
                uvs.push_back(Vec2(u1, v1));

                uvs.push_back(Vec2(u1, v1));
                uvs.push_back(Vec2(u0, v1));
                uvs.push_back(Vec2(u0, v0));
            }
        }

        return resourceManager.CreateResource<Mesh>(ID, vertices, uvs, normals);
    }
}