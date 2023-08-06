#include "Enginepch.h"
#include "Mesh.h"

namespace Iberus {
	bool Mesh::Load(Buffer inboundBuffer) {
		std::vector<Vec3> vertexData, normalData;
		std::vector<Vec2> uvData;
		std::vector<unsigned int> vertexIdx, uvIdx, normalIdx;

		auto parseVec2 = [](std::stringstream& stringIn) {
			Vec2 v;
			stringIn >> v.x >> v.y;
			return v;
		};

		auto parseVec3 = [](std::stringstream& stringIn) {
			Vec3 v;
			stringIn >> v.x >> v.y >> v.z;
			return v;
		};

		// Read from buffer
		std::ifstream ifile(reinterpret_cast<const char*>(inboundBuffer.data.get()), inboundBuffer.size);
		std::string line;
		while (std::getline(ifile, line)) {
			std::stringstream sline(line);

			std::string s;
			sline >> s;
			if (s.compare("v") == 0) { vertexData.push_back(parseVec3(sline)); }
			else if (s.compare("vt") == 0) { uvData.push_back(parseVec2(sline)); }
			else if (s.compare("vn") == 0) { normalData.push_back(parseVec3(sline)); }
			else if (s.compare("f") == 0) { 
				/// Parse Face
				std::string token;
				if (normalData.empty() && uvData.empty()) {
					for (int i = 0; i < 3; i++) {
						sline >> token;
						vertexIdx.push_back(std::stoi(token));
					}
				}
				else {
					for (int i = 0; i < 3; i++) {
						std::getline(sline, token, '/');
						if (token.size() > 0) vertexIdx.push_back(std::stoi(token));
						std::getline(sline, token, '/');
						if (token.size() > 0) uvData.push_back(std::stoi(token));
						std::getline(sline, token, ' ');
						if (token.size() > 0) normalIdx.push_back(std::stoi(token));
					}
				}
			}
		}
		
		hasUVs = (uvIdx.size() > 0);
		hasNormals = (normalIdx.size() > 0);

		// Process data read
		for (unsigned int i = 0; i < vertexIdx.size(); i++) {
			unsigned int vi = vertexIdx[i];
			Vec3 v = vertexData[vi - 1];
			vertices.push_back(v);
			if (hasUVs) {
				unsigned int uvi = uvIdx[i];
				Vec2 uv = uvData[uvi - 1];
				uvs.push_back(uv);
			}
			if (hasNormals) {
				unsigned int ni = normalIdx[i];
				Vec3 n = normalData[ni - 1];
				normals.push_back(n);
			}
		}

		return true; // TODO(MPP) This needs more load checks
	}
}