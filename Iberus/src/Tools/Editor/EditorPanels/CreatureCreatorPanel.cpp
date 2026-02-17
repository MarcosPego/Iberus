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
			const int segCount = 5;   // 5 capsule segments → 6 points
			const float span = 3.0f;
			const float radius = 0.18f;
			const std::string matId = "SDFMaterial1";
			scene->GetOrCreateMaterial<Material>(matId);
			for (int i = 0; i < segCount; ++i) {
				SDFPartData p;
				p.Type = 3; // Capsule segments read more worm-like than spheres
				float t0 = (segCount > 1) ? (i / (float)segCount) : 0.0f;
				float t1 = (segCount > 1) ? ((i + 1) / (float)segCount) : 1.0f;
				p.Transform.Position = Vec3(t0 * span - span / 2, 0, 0);
				p.Endpoint = Vec3(t1 * span - span / 2, 0, 0);
				p.Radius = radius;
				p.MaterialId = matId;
				sdf.Parts.push_back(p);
			}
		}

		void ApplySnake(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const float spineLen = 4.0f;
			const int segs = 12;
			const float spineRadius = 0.08f;
			const std::string spineMatId = "SDFMaterial1";
			const std::string eyeMatId = "SDFMaterial2";
			scene->GetOrCreateMaterial<Material>(spineMatId);
			scene->GetOrCreateMaterial<Material>(eyeMatId);

			for (int i = 0; i < segs; ++i) {
				SDFPartData p;
				p.Type = 3;
				p.Transform.Position = Vec3(i * spineLen / segs - spineLen / 2, 0, 0);
				p.Endpoint = Vec3((i + 1) * spineLen / segs - spineLen / 2, 0, 0);
				p.Radius = spineRadius;
				p.MaterialId = spineMatId;
				sdf.Parts.push_back(p);
			}

			const float headX = spineLen / 2;
			SDFPartData eye1;
			eye1.Type = 1;
			eye1.Transform.Position = Vec3(headX + 0.15f, 0.12f, 0.06f);
			eye1.Radius = 0.06f;
			eye1.MaterialId = eyeMatId;
			sdf.Parts.push_back(eye1);

			SDFPartData eye2;
			eye2.Type = 1;
			eye2.Transform.Position = Vec3(headX + 0.15f, 0.12f, -0.06f);
			eye2.Radius = 0.06f;
			eye2.MaterialId = eyeMatId;
			sdf.Parts.push_back(eye2);
		}

		void ApplyDragonBase(SDFComponent& sdf, Scene* scene) {
			sdf.Parts.clear();
			const std::string bodyMatId = "SDFMaterial1";
			const std::string limbMatId = "SDFMaterial2";
			const std::string headMatId = "SDFMaterial3";
			scene->GetOrCreateMaterial<Material>(bodyMatId);
			scene->GetOrCreateMaterial<Material>(limbMatId);
			scene->GetOrCreateMaterial<Material>(headMatId);

			// Body: elongated torso along X
			SDFPartData body1;
			body1.Type = 3;
			body1.Transform.Position = Vec3(-0.8f, 0.5f, 0);
			body1.Endpoint = Vec3(0.6f, 0.5f, 0);
			body1.Radius = 0.18f;
			body1.MaterialId = bodyMatId;
			sdf.Parts.push_back(body1);

			// Head: distinct sphere at front
			SDFPartData head;
			head.Type = 1;
			head.Transform.Position = Vec3(1.0f, 0.6f, 0);
			head.Radius = 0.18f;
			head.MaterialId = headMatId;
			sdf.Parts.push_back(head);

			// Four limb stubs - spread out, smaller so they don't blob with body
			auto limb = [&](Vec3 pos) {
				SDFPartData p;
				p.Type = 1;
				p.Transform.Position = pos;
				p.Radius = 0.08f;
				p.MaterialId = limbMatId;
				sdf.Parts.push_back(p);
			};
			limb(Vec3(0.3f, 0.25f, 0.35f));
			limb(Vec3(0.3f, 0.25f, -0.35f));
			limb(Vec3(-0.3f, 0.25f, 0.35f));
			limb(Vec3(-0.3f, 0.25f, -0.35f));

			// Tail: thin tapering capsule to the rear
			SDFPartData tail;
			tail.Type = 3;
			tail.Transform.Position = Vec3(-0.8f, 0.45f, 0);
			tail.Endpoint = Vec3(-1.6f, 0.2f, 0);
			tail.Radius = 0.1f;
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
