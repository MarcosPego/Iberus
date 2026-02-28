#include "Enginepch.h"
#include "CreatureCreatorPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Components.h"
#include "Material.h"
#include "MathUtils.h"

#include "imgui.h"

using namespace Math;

namespace Iberus {

	namespace {

		void ApplyWorm(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const int segCount = 5;
			const float span = 3.0f;
			const float radius = 0.18f;
			const std::string bodyMatId = "SDFMaterial1";
			const std::string eyeMatId = "SDFMaterial2";
			{
				auto& rm = Engine::Instance()->GetResourceManager();
				auto* prov = &Engine::Instance()->GetEngineProvider();
				rm.GetOrCreateResource<Material>(bodyMatId, prov);
				rm.GetOrCreateResource<Material>(eyeMatId, prov);
			}

			for (int i = 0; i < segCount; ++i) {
				SDFPartData p;
				p.Type = 3;
				float t0 = (segCount > 1) ? (i / (float)segCount) : 0.0f;
				float t1 = (segCount > 1) ? ((i + 1) / (float)segCount) : 1.0f;
				p.Transform.Position = Vec3(t0 * span - span / 2, 0, 0);
				p.Endpoint = Vec3(t1 * span - span / 2, 0, 0);
				p.Radius = radius;
				p.MaterialId = bodyMatId;
				sdf.Parts.push_back(p);
			}

			// Eyes at head (front)
			const float headX = span / 2;
			auto eye = [&](float z) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = Vec3(headX + 0.12f, 0.1f, z);
				p.Radius = 0.05f;
				p.MaterialId = eyeMatId;
				sdf.Parts.push_back(p);
			};
			eye(0.07f);
			eye(-0.07f);
		}

		void ApplySnake(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const float spineLen = 4.0f;
			const int segs = 10;
			const float spineRadius = 0.08f;
			const std::string spineMatId = "SDFMaterial1";
			const std::string headMatId = "SDFMaterial2";
			const std::string eyeMatId = "SDFMaterial3";
			{
				auto& rm = Engine::Instance()->GetResourceManager();
				auto* prov = &Engine::Instance()->GetEngineProvider();
				rm.GetOrCreateResource<Material>(spineMatId, prov);
				rm.GetOrCreateResource<Material>(headMatId, prov);
				rm.GetOrCreateResource<Material>(eyeMatId, prov);
			}

			for (int i = 0; i < segs; ++i) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = Vec3(i * spineLen / segs - spineLen / 2, 0, 0);
				p.Endpoint = Vec3((i + 1) * spineLen / segs - spineLen / 2, 0, 0);
				p.Radius = spineRadius;
				p.MaterialId = spineMatId;
				sdf.Parts.push_back(p);
			}

			// Head bulge - distinguishes head from body
			const float headX = spineLen / 2;
			SDFPartData head;
			head.Type = 1;
			head.Transform.Position = Vec3(headX + 0.1f, 0.08f, 0);
			head.Radius = 0.12f;
			head.MaterialId = headMatId;
			sdf.Parts.push_back(head);

			// Eyes - larger, on top of head
			auto eye = [&](float z) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = Vec3(headX + 0.2f, 0.18f, z);
				p.Radius = 0.07f;
				p.MaterialId = eyeMatId;
				sdf.Parts.push_back(p);
			};
			eye(0.08f);
			eye(-0.08f);

			// Forked tongue
			auto tongueSeg = [&](Vec3 from, Vec3 to) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = from;
				p.Endpoint = to;
				p.Radius = 0.02f;
				p.MaterialId = headMatId;
				sdf.Parts.push_back(p);
			};
			Vec3 tongueBase(headX + 0.25f, 0.05f, 0);
			tongueSeg(tongueBase, Vec3(headX + 0.45f, 0.02f, 0.04f));
			tongueSeg(tongueBase, Vec3(headX + 0.45f, 0.02f, -0.04f));
		}

		void ApplyDragonBase(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const std::string bodyMatId = "SDFMaterial1";
			const std::string limbMatId = "SDFMaterial2";
			const std::string headMatId = "SDFMaterial3";
			const std::string wngMatId = "SDFMaterial2";
			const std::string eyeMatId = "SDFMaterial3";
			constexpr uint32_t bodyMask = 0x01u;
			constexpr uint32_t leg0Mask = 0x02u, leg1Mask = 0x04u, leg2Mask = 0x08u, leg3Mask = 0x10u;
			constexpr uint32_t wngLMask = 0x20u, wngRMask = 0x40u;
			const float scale = 2.5f;
			const float rScale = 0.75f;
			{
				auto& rm = Engine::Instance()->GetResourceManager();
				auto* prov = &Engine::Instance()->GetEngineProvider();
				rm.GetOrCreateResource<Material>(bodyMatId, prov);
				rm.GetOrCreateResource<Material>(limbMatId, prov);
				rm.GetOrCreateResource<Material>(headMatId, prov);
				rm.GetOrCreateResource<Material>(wngMatId, prov);
				rm.GetOrCreateResource<Material>(eyeMatId, prov);
			}

			auto addBody = [&](SDFPartData p) {
				p.BlendGroupMask = bodyMask;
				sdf.Parts.push_back(p);
			};

			const float bodyR = 0.22f * rScale;
			const float neckR = 0.14f * rScale;
			const float legJointR = 0.18f * rScale;
			const float legR = 0.14f * rScale;
			const float wngJointR = 0.2f * rScale;
			const float wngR = 0.16f * rScale;

			// Body: torso with radius large enough for legs to attach
			const float bodyY = 1.2f * scale;
			SDFPartData body;
			body.Type = 3;
			body.Transform.Position = Vec3(-1.8f * scale, bodyY, 0);
			body.Endpoint = Vec3(1.2f * scale, bodyY, 0);
			body.Radius = bodyR;
			body.MaterialId = bodyMatId;
			addBody(body);

			// Neck: overlaps body at joint (dist <= neckR + bodyR)
			const float neckStartY = bodyY;
			const float neckEndY = 2.2f * scale;
			SDFPartData neck;
			neck.Type = 3;
			neck.Transform.Position = Vec3(1.2f * scale, neckStartY, 0);
			neck.Endpoint = Vec3(2.6f * scale, neckEndY, 0);
			neck.Radius = neckR;
			neck.MaterialId = bodyMatId;
			addBody(neck);

			// Head: overlaps neck (dist from neck endpoint <= headR + neckR)
			const float headY = 2.5f * scale;
			SDFPartData head;
			head.Type = 1;
			head.Transform.Position = Vec3(2.9f * scale, headY, 0);
			head.Radius = 0.2f * rScale;
			head.MaterialId = headMatId;
			addBody(head);

			// Snout: starts at head center, overlaps (head bounds touch snout)
			SDFPartData snout;
			snout.Type = 3;
			snout.Transform.Position = Vec3(2.9f * scale, headY - 0.05f * scale, 0);
			snout.Endpoint = Vec3(3.8f * scale, 2.0f * scale, 0);
			snout.Radius = 0.12f * rScale;
			snout.MaterialId = headMatId;
			addBody(snout);

			// Eyes: on head surface
			auto eye = [&](float z) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = Vec3(2.8f * scale, headY + 0.15f * scale, z * scale);
				p.Radius = 0.1f * rScale;
				p.MaterialId = eyeMatId;
				addBody(p);
			};
			eye(0.2f);
			eye(-0.2f);

			// Leg joints ON body surface: center at body bottom (bodyY - bodyR), joint sphere overlaps body
			// dist(joint, body_axis) <= bodyR for joint to touch body. Leg splay via foot offset.
			const float legJointY = bodyY - bodyR;
			auto legJoint = [&](Vec3 pos, uint32_t mask) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = pos;
				p.Radius = legJointR;
				p.MaterialId = limbMatId;
				p.BlendGroupMask = bodyMask | mask;
				sdf.Parts.push_back(p);
			};
			auto leg = [&](Vec3 shoulder, Vec3 foot, uint32_t mask) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = shoulder;
				p.Endpoint = foot;
				p.Radius = legR;
				p.MaterialId = limbMatId;
				p.BlendGroupMask = mask;
				sdf.Parts.push_back(p);
			};
			const float legOffsetZ = bodyR * 0.8f;
			Vec3 leg0Joint(0.5f * scale, legJointY, legOffsetZ);
			legJoint(leg0Joint, leg0Mask);
			leg(leg0Joint, Vec3(0.7f * scale, -0.4f * scale, legOffsetZ + 0.9f * scale), leg0Mask);
			Vec3 leg1Joint(0.5f * scale, legJointY, -legOffsetZ);
			legJoint(leg1Joint, leg1Mask);
			leg(leg1Joint, Vec3(0.7f * scale, -0.4f * scale, -legOffsetZ - 0.9f * scale), leg1Mask);
			Vec3 leg2Joint(-1.0f * scale, legJointY, legOffsetZ);
			legJoint(leg2Joint, leg2Mask);
			leg(leg2Joint, Vec3(-1.2f * scale, -0.4f * scale, legOffsetZ + 0.9f * scale), leg2Mask);
			Vec3 leg3Joint(-1.0f * scale, legJointY, -legOffsetZ);
			legJoint(leg3Joint, leg3Mask);
			leg(leg3Joint, Vec3(-1.2f * scale, -0.4f * scale, -legOffsetZ - 0.9f * scale), leg3Mask);

			auto wngJoint = [&](Vec3 pos, uint32_t mask) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = pos;
				p.Radius = wngJointR;
				p.MaterialId = wngMatId;
				p.BlendGroupMask = bodyMask | mask;
				sdf.Parts.push_back(p);
			};
			auto wngSeg = [&](Vec3 from, Vec3 to, uint32_t mask) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = from;
				p.Endpoint = to;
				p.Radius = wngR;
				p.MaterialId = wngMatId;
				p.BlendGroupMask = mask;
				sdf.Parts.push_back(p);
			};

			// Wings: joint on body upper surface (within bodyR of axis)
			const float wngY = bodyY + bodyR * 0.7f;
			const float wngZ = bodyR * 0.7f;
			const float wngStep = wngR * 1.8f;
			Vec3 wngL0(-0.2f * scale, wngY, wngZ);
			Vec3 wngL1 = wngL0 + Vec3(-wngStep, wngStep * 0.6f, wngStep * 0.5f);
			Vec3 wngL2 = wngL1 + Vec3(-wngStep, wngStep * 0.3f, -wngStep * 0.2f);
			Vec3 wngL3 = wngL2 + Vec3(-wngStep, -wngStep * 0.2f, -wngStep * 0.3f);
			wngJoint(wngL0, wngLMask);
			wngSeg(wngL0, wngL1, wngLMask);
			wngSeg(wngL1, wngL2, wngLMask);
			wngSeg(wngL2, wngL3, wngLMask);
			Vec3 wngR0(-0.2f * scale, wngY, -wngZ);
			Vec3 wngR1 = wngR0 + Vec3(-wngStep, wngStep * 0.6f, -wngStep * 0.5f);
			Vec3 wngR2 = wngR1 + Vec3(-wngStep, wngStep * 0.3f, wngStep * 0.2f);
			Vec3 wngR3 = wngR2 + Vec3(-wngStep, -wngStep * 0.2f, wngStep * 0.3f);
			wngJoint(wngR0, wngRMask);
			wngSeg(wngR0, wngR1, wngRMask);
			wngSeg(wngR1, wngR2, wngRMask);
			wngSeg(wngR2, wngR3, wngRMask);

			// Tail: overlaps body at start
			SDFPartData tail;
			tail.Type = 3;
			tail.Transform.Position = Vec3(-1.8f * scale, bodyY, 0);
			tail.Endpoint = Vec3(-4.5f * scale, 0.3f * scale, 0);
			tail.Radius = 0.14f * rScale;
			tail.MaterialId = bodyMatId;
			addBody(tail);
		}

		void ApplyFish(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const std::string bodyMatId = "SDFMaterial1";
			const std::string finMatId = "SDFMaterial2";
			const std::string eyeMatId = "SDFMaterial3";
			constexpr uint32_t bodyMask = 0x01u;
			constexpr uint32_t dorsalMask = 0x02u;
			constexpr uint32_t pectoralLMask = 0x04u, pectoralRMask = 0x08u;
			{
				auto& rm = Engine::Instance()->GetResourceManager();
				auto* prov = &Engine::Instance()->GetEngineProvider();
				rm.GetOrCreateResource<Material>(bodyMatId, prov);
				rm.GetOrCreateResource<Material>(finMatId, prov);
				rm.GetOrCreateResource<Material>(eyeMatId, prov);
			}

			auto addBody = [&](SDFPartData p) {
				p.BlendGroupMask = bodyMask;
				sdf.Parts.push_back(p);
			};

			// Body: 5 capsule segments along +X
			const float bodyLen = 2.5f;
			const int segCount = 5;
			for (int i = 0; i < segCount; ++i) {
				SDFPartData p;
				p.Type = 3;
				float t0 = (segCount > 1) ? (i / (float)segCount) : 0.0f;
				float t1 = (segCount > 1) ? ((i + 1) / (float)segCount) : 1.0f;
				p.Transform.Position = Vec3(t0 * bodyLen - bodyLen / 2, 0, 0);
				p.Endpoint = Vec3(t1 * bodyLen - bodyLen / 2, 0, 0);
				p.Radius = 0.12f - i * 0.01f;
				p.MaterialId = bodyMatId;
				addBody(p);
			}

			// Head sphere
			const float headX = bodyLen / 2;
			SDFPartData head;
			head.Type = 1;
			head.Transform.Position = Vec3(headX + 0.15f, 0.02f, 0);
			head.Radius = 0.14f;
			head.MaterialId = bodyMatId;
			addBody(head);

			// Tail fin (forked, 2 thin capsules)
			Vec3 tailBase(-bodyLen / 2 - 0.1f, 0, 0);
			auto tailFin = [&](Vec3 to) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = tailBase;
				p.Endpoint = to;
				p.Radius = 0.04f;
				p.MaterialId = finMatId;
				addBody(p);
			};
			tailFin(Vec3(-bodyLen / 2 - 0.5f, 0.08f, 0.08f));
			tailFin(Vec3(-bodyLen / 2 - 0.5f, 0.08f, -0.08f));

			// Dorsal fin with joint
			SDFPartData dorsalJoint;
			dorsalJoint.Type = 1;
			dorsalJoint.Transform.Position = Vec3(0.1f, 0.14f, 0);
			dorsalJoint.Radius = 0.06f;
			dorsalJoint.MaterialId = finMatId;
			dorsalJoint.BlendGroupMask = bodyMask | dorsalMask;
			sdf.Parts.push_back(dorsalJoint);

			SDFPartData dorsal;
			dorsal.Type = 3;
			dorsal.Transform.Position = Vec3(0.1f, 0.14f, 0);
			dorsal.Endpoint = Vec3(0.05f, 0.35f, 0);
			dorsal.Radius = 0.05f;
			dorsal.MaterialId = finMatId;
			dorsal.BlendGroupMask = dorsalMask;
			sdf.Parts.push_back(dorsal);

			// Pectoral fins with joints
			auto pectoralJoint = [&](Vec3 pos, uint32_t mask) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = pos;
				p.Radius = 0.05f;
				p.MaterialId = finMatId;
				p.BlendGroupMask = bodyMask | mask;
				sdf.Parts.push_back(p);
			};
			auto pectoral = [&](Vec3 from, Vec3 to, uint32_t mask) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = from;
				p.Endpoint = to;
				p.Radius = 0.04f;
				p.MaterialId = finMatId;
				p.BlendGroupMask = mask;
				sdf.Parts.push_back(p);
			};
			Vec3 pectoralLFrom(headX - 0.2f, 0.02f, 0.1f);
			pectoralJoint(pectoralLFrom, pectoralLMask);
			pectoral(pectoralLFrom, Vec3(headX - 0.5f, -0.05f, 0.18f), pectoralLMask);
			Vec3 pectoralRFrom(headX - 0.2f, 0.02f, -0.1f);
			pectoralJoint(pectoralRFrom, pectoralRMask);
			pectoral(pectoralRFrom, Vec3(headX - 0.5f, -0.05f, -0.18f), pectoralRMask);

			// Eyes
			auto eye = [&](float z) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = Vec3(headX + 0.12f, 0.08f, z);
				p.Radius = 0.05f;
				p.MaterialId = eyeMatId;
				addBody(p);
			};
			eye(0.06f);
			eye(-0.06f);
		}

		void ApplyClear(SDFComponent& sdf) {
			sdf.Parts.clear();
		}

	} // namespace

	CreatureCreatorPanel::CreatureCreatorPanel(Editor& editor) : editor(editor) {
	}

	void CreatureCreatorPanel::OnDraw(IGUIContext& gui) {
		if (!gui.BeginWindow("Creature Creator")) {
			gui.EndWindow();
			return;
		}

		EntityId entityId = editor.GetSelectedEntityId();
		auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
		if (!scene || entityId == NullEntity || !scene->GetWorld().IsAlive(entityId)) {
			gui.Text("Select an entity");
			gui.EndWindow();
			return;
		}

		auto* sdf = scene->GetWorld().GetComponent<SDFComponent>(entityId);
		if (!sdf) {
			gui.Text("Select an SDF entity");
			gui.EndWindow();
			return;
		}

		if (ImGui::CollapsingHeader("Creature Creator##CreatureCreatorHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Presets");
			if (ImGui::Button("Worm##PresetWorm")) {
				ApplyWorm(*sdf, scene);
				scene->OnEntityChanged().Invoke(entityId);
			}
			ImGui::SameLine();
			if (ImGui::Button("Snake##PresetSnake")) {
				ApplySnake(*sdf, scene);
				scene->OnEntityChanged().Invoke(entityId);
			}
			ImGui::SameLine();
			if (ImGui::Button("Fish##PresetFish")) {
				ApplyFish(*sdf, scene);
				scene->OnEntityChanged().Invoke(entityId);
			}
			ImGui::SameLine();
			if (ImGui::Button("Dragon Base##PresetDragon")) {
				ApplyDragonBase(*sdf, scene);
				scene->OnEntityChanged().Invoke(entityId);
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear##PresetClear")) {
				ApplyClear(*sdf);
				scene->OnEntityChanged().Invoke(entityId);
			}

			ImGui::Separator();
			ImGui::Text("Parts (%zu)", sdf->Parts.size());

			for (size_t i = 0; i < sdf->Parts.size(); ++i) {
				auto& part = sdf->Parts[i];
				ImGui::PushID(static_cast<int>(i));

				ImGui::Text("Part %zu", i);

				if (ImGui::Button("Up##SDFPartUp")) {
					if (i > 0) {
						std::swap(sdf->Parts[i], sdf->Parts[i - 1]);
						scene->OnEntityChanged().Invoke(entityId);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Down##SDFPartDown")) {
					if (i + 1 < sdf->Parts.size()) {
						std::swap(sdf->Parts[i], sdf->Parts[i + 1]);
						scene->OnEntityChanged().Invoke(entityId);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove##SDFPartRemove")) {
					sdf->Parts.erase(sdf->Parts.begin() + static_cast<std::ptrdiff_t>(i));
					scene->OnEntityChanged().Invoke(entityId);
					ImGui::PopID();
					break;
				}
				ImGui::SameLine();
				if (ImGui::Button("Duplicate##SDFPartDuplicate")) {
					sdf->Parts.insert(sdf->Parts.begin() + static_cast<std::ptrdiff_t>(i) + 1, part);
					scene->OnEntityChanged().Invoke(entityId);
				}

				ImGui::PopID();
			}

			ImGui::Separator();
			if (ImGui::BeginMenu("Add Part##SDFAddPartMenu")) {
				if (ImGui::MenuItem("Sphere##AddSphere")) {
					SDFPartData p;
					p.Type = 1;
					p.Radius = 0.5f;
					p.MaterialId = "SDFMaterial1";
					sdf->Parts.push_back(p);
					scene->OnEntityChanged().Invoke(entityId);
				}
				if (ImGui::MenuItem("Box##AddBox")) {
					SDFPartData p;
					p.Type = 2;
					p.Radius = 0.5f;
					p.MaterialId = "SDFMaterial1";
					sdf->Parts.push_back(p);
					scene->OnEntityChanged().Invoke(entityId);
				}
				if (ImGui::MenuItem("Capsule##AddCapsule")) {
					SDFPartData p;
					p.Type = 3;
					p.Transform.Position = Vec3(-0.5f, 0, 0);
					p.Endpoint = Vec3(0.5f, 0, 0);
					p.Radius = 0.25f;
					p.MaterialId = "SDFMaterial1";
					sdf->Parts.push_back(p);
					scene->OnEntityChanged().Invoke(entityId);
				}
				ImGui::EndMenu();
			}
		}

		gui.EndWindow();
	}

}
