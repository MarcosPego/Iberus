#pragma once

#include "Core.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	
	class Entity;
	class Camera;

	class IBERUS_API Scene {
	public:
		Scene();
		~Scene();

	private:
		Camera* activeCamera{ nullptr };
		Entity* sceneRoot{ nullptr };

		std::unordered_map<std::string, Entity*> entityMap;
	};

 }
