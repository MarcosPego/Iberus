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
			const std::string wingMatId = "SDFMaterial2";
			const std::string eyeMatId = "SDFMaterial3";
			{
				auto& rm = Engine::Instance()->GetResourceManager();
				auto* prov = &Engine::Instance()->GetEngineProvider();
				rm.GetOrCreateResource<Material>(bodyMatId, prov);
				rm.GetOrCreateResource<Material>(limbMatId, prov);
				rm.GetOrCreateResource<Material>(headMatId, prov);
				rm.GetOrCreateResource<Material>(wingMatId, prov);
				rm.GetOrCreateResource<Material>(eyeMatId, prov);
			}

			// Body: elongated torso
			SDFPartData body;
			body.Type = 3;
			body.Transform.Position = Vec3(-0.7f, 0.5f, 0);
			body.Endpoint = Vec3(0.5f, 0.5f, 0);
			body.Radius = 0.16f;
			body.MaterialId = bodyMatId;
			sdf.Parts.push_back(body);

			// Neck + head
			SDFPartData neck;
			neck.Type = 3;
			neck.Transform.Position = Vec3(0.5f, 0.5f, 0);
			neck.Endpoint = Vec3(0.95f, 0.65f, 0);
			neck.Radius = 0.1f;
			neck.MaterialId = bodyMatId;
			sdf.Parts.push_back(neck);

			SDFPartData head;
			head.Type = 1;
			head.Transform.Position = Vec3(1.1f, 0.7f, 0);
			head.Radius = 0.14f;
			head.MaterialId = headMatId;
			sdf.Parts.push_back(head);

			// Snout
			SDFPartData snout;
			snout.Type = 3;
			snout.Transform.Position = Vec3(1.1f, 0.68f, 0);
			snout.Endpoint = Vec3(1.35f, 0.65f, 0);
			snout.Radius = 0.06f;
			snout.MaterialId = headMatId;
			sdf.Parts.push_back(snout);

			// Eyes
			auto eye = [&](float z) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = Vec3(1.08f, 0.78f, z);
				p.Radius = 0.05f;
				p.MaterialId = eyeMatId;
				sdf.Parts.push_back(p);
			};
			eye(0.08f);
			eye(-0.08f);

			// Four legs (capsules for length)
			auto leg = [&](Vec3 shoulder, Vec3 foot) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = shoulder;
				p.Endpoint = foot;
				p.Radius = 0.06f;
				p.MaterialId = limbMatId;
				sdf.Parts.push_back(p);
			};
			leg(Vec3(0.2f, 0.4f, 0.28f), Vec3(0.15f, 0.05f, 0.35f));
			leg(Vec3(0.2f, 0.4f, -0.28f), Vec3(0.15f, 0.05f, -0.35f));
			leg(Vec3(-0.4f, 0.4f, 0.28f), Vec3(-0.45f, 0.05f, 0.35f));
			leg(Vec3(-0.4f, 0.4f, -0.28f), Vec3(-0.45f, 0.05f, -0.35f));

			// Wings - each wing is 2 segments (upper arm, wing membrane)
			auto wingSeg = [&](Vec3 from, Vec3 to) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = from;
				p.Endpoint = to;
				p.Radius = 0.08f;
				p.MaterialId = wingMatId;
				sdf.Parts.push_back(p);
			};
			// Left wing
			wingSeg(Vec3(0.0f, 0.55f, 0.35f), Vec3(-0.15f, 0.85f, 0.4f));
			wingSeg(Vec3(-0.15f, 0.85f, 0.4f), Vec3(-0.4f, 0.75f, 0.3f));
			// Right wing
			wingSeg(Vec3(0.0f, 0.55f, -0.35f), Vec3(-0.15f, 0.85f, -0.4f));
			wingSeg(Vec3(-0.15f, 0.85f, -0.4f), Vec3(-0.4f, 0.75f, -0.3f));

			// Tail
			SDFPartData tail;
			tail.Type = 3;
			tail.Transform.Position = Vec3(-0.7f, 0.45f, 0);
			tail.Endpoint = Vec3(-1.5f, 0.15f, 0);
			tail.Radius = 0.08f;
			tail.MaterialId = bodyMatId;
			sdf.Parts.push_back(tail);
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
			}
			ImGui::SameLine();
			if (ImGui::Button("Snake##PresetSnake")) {
				ApplySnake(*sdf, scene);
			}
			ImGui::SameLine();
			if (ImGui::Button("Dragon Base##PresetDragon")) {
				ApplyDragonBase(*sdf, scene);
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear##PresetClear")) {
				ApplyClear(*sdf);
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
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Down##SDFPartDown")) {
					if (i + 1 < sdf->Parts.size()) {
						std::swap(sdf->Parts[i], sdf->Parts[i + 1]);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove##SDFPartRemove")) {
					sdf->Parts.erase(sdf->Parts.begin() + static_cast<std::ptrdiff_t>(i));
					ImGui::PopID();
					break;
				}
				ImGui::SameLine();
				if (ImGui::Button("Duplicate##SDFPartDuplicate")) {
					sdf->Parts.insert(sdf->Parts.begin() + static_cast<std::ptrdiff_t>(i) + 1, part);
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
				}
				if (ImGui::MenuItem("Box##AddBox")) {
					SDFPartData p;
					p.Type = 2;
					p.Radius = 0.5f;
					p.MaterialId = "SDFMaterial1";
					sdf->Parts.push_back(p);
				}
				if (ImGui::MenuItem("Capsule##AddCapsule")) {
					SDFPartData p;
					p.Type = 3;
					p.Transform.Position = Vec3(-0.5f, 0, 0);
					p.Endpoint = Vec3(0.5f, 0, 0);
					p.Radius = 0.25f;
					p.MaterialId = "SDFMaterial1";
					sdf->Parts.push_back(p);
				}
				ImGui::EndMenu();
			}
		}

		gui.EndWindow();
	}

}
