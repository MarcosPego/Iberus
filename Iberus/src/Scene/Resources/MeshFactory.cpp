#include "Enginepch.h"
#include "MeshFactory.h"

#include "MathUtils.h"
using namespace Math;

namespace Iberus {

	Mesh* MeshFactory::CreateCube(const std::string& ID, ResourceManager& resourceManager) {
		auto* mesh = resourceManager.GetResource<Mesh>(ID);
		if (mesh) {
			return mesh;
		}
		// 1x1x1 cube centered at origin. 8 vertices, 12 triangles (2 per face).
		const float h = 0.5f;
		std::vector<Vec3> vertices = {
			{-h,-h,-h}, {-h, h,-h}, { h, h,-h}, { h,-h,-h},  // back
			{-h,-h, h}, { h,-h, h}, { h, h, h}, {-h, h, h},  // front
			{-h,-h,-h}, { h,-h,-h}, { h,-h, h}, {-h,-h, h},  // bottom
			{-h, h,-h}, {-h, h, h}, { h, h, h}, { h, h,-h},  // top
			{-h,-h,-h}, {-h,-h, h}, {-h, h, h}, {-h, h,-h},  // left
			{ h,-h,-h}, { h, h,-h}, { h, h, h}, { h,-h, h}   // right
		};
		std::vector<Vec3> normals = {
			{ 0, 0,-1.0f}, { 0, 0,-1.0f}, { 0, 0,-1.0f}, { 0, 0,-1.0f},
			{ 0, 0, 1.0f}, { 0, 0, 1.0f}, { 0, 0, 1.0f}, { 0, 0, 1.0f},
			{ 0,-1.0f, 0}, { 0,-1.0f, 0}, { 0,-1.0f, 0}, { 0,-1.0f, 0},
			{ 0, 1.0f, 0}, { 0, 1.0f, 0}, { 0, 1.0f, 0}, { 0, 1.0f, 0},
			{-1.0f, 0, 0}, {-1.0f, 0, 0}, {-1.0f, 0, 0}, {-1.0f, 0, 0},
			{ 1.0f, 0, 0}, { 1.0f, 0, 0}, { 1.0f, 0, 0}, { 1.0f, 0, 0}
		};
		std::vector<Vec2> uvs(24, Vec2(0, 0));
		for (int i = 0; i < 6; ++i) {
			uvs[i*4 + 0] = Vec2(0, 0);
			uvs[i*4 + 1] = Vec2(1, 0);
			uvs[i*4 + 2] = Vec2(1, 1);
			uvs[i*4 + 3] = Vec2(0, 1);
		}
		std::vector<Vec3> triVertices, triNormals;
		std::vector<Vec2> triUvs;
		for (int f = 0; f < 6; ++f) {
			int b = f * 4;
			triVertices.insert(triVertices.end(), { vertices[b], vertices[b+1], vertices[b+2], vertices[b+2], vertices[b+3], vertices[b] });
			triNormals.insert(triNormals.end(), { normals[b], normals[b+1], normals[b+2], normals[b+2], normals[b+3], normals[b] });
			triUvs.insert(triUvs.end(), { uvs[b], uvs[b+1], uvs[b+2], uvs[b+2], uvs[b+3], uvs[b] });
		}
		return resourceManager.CreateResource<Mesh>(ID, triVertices, triUvs, triNormals);
	}

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

	Mesh* MeshFactory::CreateNDCQuad(const std::string& ID, ResourceManager& resourceManager) {
		auto* mesh = resourceManager.GetResource<Mesh>(ID);
		if (mesh) {
			return mesh;
		}

		std::vector<Vec3> vertices = {
			{ -1.0f, -1.0f, 0.0f },
			{ -1.0f,  1.0f, 0.0f },
			{  1.0f,  1.0f, 0.0f },

			{  1.0f,  1.0f, 0.0f },
			{  1.0f, -1.0f, 0.0f },
			{ -1.0f, -1.0f, 0.0f }
		};

		std::vector<Vec3> normals = {
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f }
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

    Mesh* MeshFactory::CreatePlane(const std::string& ID, ResourceManager& resourceManager, int width, int height,
        const std::vector<float>& heightMap, float heightScale, float worldSizeX, float worldSizeZ) {
        const size_t expectedSize = static_cast<size_t>(width) * height;
        if (width < 2 || height < 2 || heightMap.size() != expectedSize) {
            return nullptr;
        }

        const float halfX = worldSizeX * 0.5f;
        const float halfZ = worldSizeZ * 0.5f;
        const float stepX = worldSizeX / (width - 1);
        const float stepZ = worldSizeZ / (height - 1);

        auto heightAt = [&](int ix, int iz) {
            return heightMap[iz * width + ix] * heightScale;
        };

        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Vec2> uvs;

        for (int iz = 0; iz < height - 1; iz++) {
            for (int ix = 0; ix < width - 1; ix++) {
                const float x0 = ix * stepX - halfX;
                const float x1 = (ix + 1) * stepX - halfX;
                const float z0 = iz * stepZ - halfZ;
                const float z1 = (iz + 1) * stepZ - halfZ;

                const float y00 = heightAt(ix, iz);
                const float y10 = heightAt(ix + 1, iz);
                const float y11 = heightAt(ix + 1, iz + 1);
                const float y01 = heightAt(ix, iz + 1);

                const Vec3 v00(x0, y00, z0);
                const Vec3 v10(x1, y10, z0);
                const Vec3 v11(x1, y11, z1);
                const Vec3 v01(x0, y01, z1);

                // Triangle 1: v00,v10,v11. Triangle 2: v11,v01,v00.
                // Use cross(B-A, C-A) for normal; swap order so normal points +Y (up) for terrain.
                Vec3 n0 = normalize(cross(v11 - v00, v10 - v00));
                Vec3 n1 = normalize(cross(v00 - v11, v01 - v11));

                vertices.push_back(v00);
                vertices.push_back(v10);
                vertices.push_back(v11);
                normals.push_back(n0);
                normals.push_back(n0);
                normals.push_back(n0);

                vertices.push_back(v11);
                vertices.push_back(v01);
                vertices.push_back(v00);
                normals.push_back(n1);
                normals.push_back(n1);
                normals.push_back(n1);

                const float u0 = static_cast<float>(ix) / (width - 1);
                const float u1 = static_cast<float>(ix + 1) / (width - 1);
                const float v0 = static_cast<float>(iz) / (height - 1);
                const float v1 = static_cast<float>(iz + 1) / (height - 1);

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