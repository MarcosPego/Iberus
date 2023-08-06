#pragma once

#include "Core.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	
	class Entity;
	class Camera;
	class Frame;

	class IBERUS_API Scene {
	public:
		Scene();
		~Scene();

		void Update(double deltaTime);
		void PushDraw(Frame& frame);

	private:
		Camera* activeCamera{ nullptr };
		Entity* sceneRoot{ nullptr };

		std::unordered_map<std::string, Entity*> entityMap; // Not sure of its use for now
	};

 }
