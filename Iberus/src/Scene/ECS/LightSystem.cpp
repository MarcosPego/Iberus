#include "Enginepch.h"
#include "LightSystem.h"
#include "Components.h"
#include "World.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Vector.h"

using namespace Math;

namespace Iberus {

	static Vec3 ExtractPosition(const Mat4& m) {
		return Vec3(m.data[12], m.data[13], m.data[14]);
	}

	// Transform direction by rotation part of matrix (use w=0 so translation is ignored)
	static Vec3 TransformDirection(const Mat4& m, const Vec3& dir) {
		Vec4 d(dir.x, dir.y, dir.z, 0.0f);
		Vec4 result = m * d;
		return normalize(Vec3(result.x, result.y, result.z));
	}

	void LightSystem::Execute(World& world, RenderBatch& renderBatch) {
		auto* lightStorage = world.GetStorage<LightComponent>();
		auto* transformStorage = world.GetStorage<TransformComponent>();
		auto* l2wStorage = world.GetStorage<LocalToWorldComponent>();
		auto* activeStorage = world.GetStorage<ActiveComponent>();
		if (!lightStorage || !transformStorage) {
			return;
		}

		auto lightsCmd = std::make_unique<LightsRenderCmd>();

		for (auto [entityId, light] : *lightStorage) {
			if (activeStorage) {
				auto* active = activeStorage->Get(entityId);
				if (active && !active->Active) {
					continue;
				}
			}

			Vec3 position;
			Vec3 direction = light.Direction;
			if (l2wStorage) {
				auto* l2w = l2wStorage->Get(entityId);
				if (l2w) {
					position = ExtractPosition(l2w->Matrix);
					// Directional and spot lights: direction from entity's world rotation
					if (light.Type == LightType::Directional || light.Type == LightType::Spot) {
						direction = TransformDirection(l2w->Matrix, light.Direction);
					}
				} else {
					auto* transform = transformStorage->Get(entityId);
					position = transform ? transform->Position : Vec3(0, 0, 0);
					if (light.Type == LightType::Directional || light.Type == LightType::Spot) {
						direction = normalize(light.Direction);
					}
				}
			} else {
				auto* transform = transformStorage->Get(entityId);
				position = transform ? transform->Position : Vec3(0, 0, 0);
				if (light.Type == LightType::Directional || light.Type == LightType::Spot) {
					direction = normalize(light.Direction);
				}
			}

			LightData data;
			data.Type = static_cast<int>(light.Type);
			data.Color = light.Color;
			data.Intensity = light.Intensity;
			data.Position = position;
			data.Constant = light.Constant;
			data.Linear = light.Linear;
			data.Quadratic = light.Quadratic;
			data.Angel = light.SpotAngle;
			data.CutoffDiameter = light.SpotCutoffDiameter;
			data.Range = light.Range;
			data.Direction = direction;

			lightsCmd->Lights.push_back(data);
		}

		if (!lightsCmd->Lights.empty()) {
			renderBatch.PushRenderCmdToQueue(std::move(lightsCmd), CMDQueue::Light);
		}
	}

}
