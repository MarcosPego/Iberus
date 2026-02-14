#include "Enginepch.h"
#include "LightSystem.h"
#include "Components.h"
#include "World.h"
#include "RenderBatch.h"
#include "RenderCmd.h"

using namespace Math;

namespace Iberus {

	static Vec3 ExtractPosition(const Mat4& m) {
		return Vec3(m.data[12], m.data[13], m.data[14]);
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
			if (l2wStorage) {
				auto* l2w = l2wStorage->Get(entityId);
				if (l2w) {
					position = ExtractPosition(l2w->Matrix);
				} else {
					auto* transform = transformStorage->Get(entityId);
					position = transform ? transform->Position : Vec3(0, 0, 0);
				}
			} else {
				auto* transform = transformStorage->Get(entityId);
				position = transform ? transform->Position : Vec3(0, 0, 0);
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
			data.Direction = light.Direction;

			lightsCmd->Lights.push_back(data);
		}

		if (!lightsCmd->Lights.empty()) {
			renderBatch.PushRenderCmdToQueue(std::move(lightsCmd), CMDQueue::Light);
		}
	}

}
