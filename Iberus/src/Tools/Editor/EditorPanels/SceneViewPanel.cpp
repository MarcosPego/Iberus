#include "Enginepch.h"
#include "SceneViewPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Application.h"
#include "EditorPicking.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Components.h"
#include "Matrix.h"
#include "World.h"

#include "imgui.h"

namespace Iberus {

	namespace {

		const float GizmoAxisLength = 1.5f;
		const float GizmoAxisHitRadius = 12.0f;

		bool WorldToScreen(const Math::Vec3& worldPos, const Math::Mat4& view, const Math::Mat4& proj,
			float vpX, float vpY, float vpW, float vpH, float& outScreenX, float& outScreenY) {
			Math::Mat4 viewProj = proj * view;
			Math::Vec4 clip = viewProj * Math::Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
			if (clip.w <= 0.0f) {
				return false;
			}
			float ndcX = clip.x / clip.w;
			float ndcY = clip.y / clip.w;
			outScreenX = vpX + (ndcX * 0.5f + 0.5f) * vpW;
			outScreenY = vpY + (1.0f - (ndcY * 0.5f + 0.5f)) * vpH;
			return (ndcX >= -1.0f && ndcX <= 1.0f && ndcY >= -1.0f && ndcY <= 1.0f);
		}

		float PointToSegmentDist(float px, float py, float ax, float ay, float bx, float by, float& outT) {
			float dx = bx - ax;
			float dy = by - ay;
			float len2 = dx * dx + dy * dy;
			if (len2 < 1e-10f) {
				outT = 0;
				return sqrtf((px - ax) * (px - ax) + (py - ay) * (py - ay));
			}
			float t = ((px - ax) * dx + (py - ay) * dy) / len2;
			if (t < 0) { t = 0; }
			if (t > 1) { t = 1; }
			outT = t;
			float cx = ax + t * dx;
			float cy = ay + t * dy;
			return sqrtf((px - cx) * (px - cx) + (py - cy) * (py - cy));
		}

		int DrawTranslateGizmo(ImDrawList* drawList, TransformComponent* transform, const Math::Vec3& worldPos,
			const Math::Mat4& view, const Math::Mat4& proj,
			float vpX, float vpY, float vpW, float vpH,
			float mouseX, float mouseY, bool mouseDown) {
			static int draggingAxis = -1;
			static float prevMouseX = 0, prevMouseY = 0;

			Math::Vec3 pos = worldPos;
			Math::Vec3 axes[3] = { {1,0,0}, {0,1,0}, {0,0,1} };
			ImU32 colors[3] = { IM_COL32(200,60,60,255), IM_COL32(60,200,60,255), IM_COL32(60,60,200,255) };

			float origSx, origSy;
			if (!WorldToScreen(pos, view, proj, vpX, vpY, vpW, vpH, origSx, origSy)) {
				return -1;
			}

			float endSx[3], endSy[3];
			for (int i = 0; i < 3; ++i) {
				Math::Vec3 end = pos + axes[i] * GizmoAxisLength;
				WorldToScreen(end, view, proj, vpX, vpY, vpW, vpH, endSx[i], endSy[i]);
				drawList->AddLine(ImVec2(origSx, origSy), ImVec2(endSx[i], endSy[i]), colors[i], 3.0f);
			}

			int hitAxis = -1;
			if (mouseDown && draggingAxis >= 0) {
				hitAxis = draggingAxis;
				float ax = origSx, ay = origSy;
				float bx = endSx[hitAxis], by = endSy[hitAxis];
				float sx = bx - ax, sy = by - ay;
				float len = sqrtf(sx * sx + sy * sy);
				if (len > 1e-6f) {
					float nx = sx / len;
					float ny = sy / len;
					float deltaPixels = (mouseX - prevMouseX) * nx + (mouseY - prevMouseY) * ny;
					float worldPerPixel = GizmoAxisLength / len;
					float deltaWorld = deltaPixels * worldPerPixel;
					transform->Position = transform->Position + axes[hitAxis] * deltaWorld;
				}
			} else {
				float bestDist = GizmoAxisHitRadius;
				for (int i = 0; i < 3; ++i) {
					float t;
					float d = PointToSegmentDist(mouseX, mouseY, origSx, origSy, endSx[i], endSy[i], t);
					if (d < bestDist && t > 0.05f) {
						bestDist = d;
						hitAxis = i;
					}
				}
				if (mouseDown && hitAxis >= 0) {
					draggingAxis = hitAxis;
				} else if (!mouseDown) {
					draggingAxis = -1;
				}
			}
			prevMouseX = mouseX;
			prevMouseY = mouseY;
			return hitAxis;
		}

		void DrawSelectionAABB(ImDrawList* drawList, const Iberus::World& world, Iberus::EntityId entityId,
			const Math::Mat4& view, const Math::Mat4& proj,
			float vpX, float vpY, float vpW, float vpH) {
			Math::Vec3 aabbMin, aabbMax;
			GetEntityAABB(world, entityId, aabbMin, aabbMax);
			Math::Vec3 corners[8] = {
				{aabbMin.x, aabbMin.y, aabbMin.z}, {aabbMax.x, aabbMin.y, aabbMin.z},
				{aabbMax.x, aabbMax.y, aabbMin.z}, {aabbMin.x, aabbMax.y, aabbMin.z},
				{aabbMin.x, aabbMin.y, aabbMax.z}, {aabbMax.x, aabbMin.y, aabbMax.z},
				{aabbMax.x, aabbMax.y, aabbMax.z}, {aabbMin.x, aabbMax.y, aabbMax.z}
			};
			float sx[8], sy[8];
			bool valid[8];
			for (int i = 0; i < 8; ++i) {
				valid[i] = WorldToScreen(corners[i], view, proj, vpX, vpY, vpW, vpH, sx[i], sy[i]);
			}
			const ImU32 color = IM_COL32(100, 180, 255, 255);
			auto addEdge = [&](int a, int b) {
				if (valid[a] && valid[b]) {
					drawList->AddLine(ImVec2(sx[a], sy[a]), ImVec2(sx[b], sy[b]), color);
				}
			};
			addEdge(0, 1); addEdge(1, 2); addEdge(2, 3); addEdge(3, 0);
			addEdge(4, 5); addEdge(5, 6); addEdge(6, 7); addEdge(7, 4);
			addEdge(0, 4); addEdge(1, 5); addEdge(2, 6); addEdge(3, 7);
		}
	}

	SceneViewPanel::SceneViewPanel(Editor& editor) : editor(editor) {
	}

	void SceneViewPanel::OnDraw(IGUIContext& gui, bool* p_open) {
		if (!ImGui::Begin("Scene View", p_open, ImGuiWindowFlags_NoNavInputs)) {
			editor.SetSceneViewFocused(false);
			ImGui::End();
			return;
		}
		editor.SetSceneViewFocused(ImGui::IsWindowHovered());

		if (Application::Get()->IsGameFullscreen()) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowCollapsed()) {
			ImGui::End();
			return;
		}

		ImVec2 size = ImGui::GetContentRegionAvail();
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);
		const int minViewportSize = 8;
		if (w < minViewportSize || h < minViewportSize || w > 16384 || h > 16384) {
			ImGui::End();
			return;
		}
		{
			if (!sceneFbo) {
				sceneFbo = std::make_unique<SceneViewFBO>();
			}
			sceneFbo->Resize(w, h);
			float aspect = static_cast<float>(w) / static_cast<float>(h);
			auto camera = editor.GetEditorCameraOverride(aspect);
			Engine::Instance()->AddEditorView(sceneFbo->GetFBO(), w, h, std::move(camera));

			ImVec2 imageMin = ImGui::GetCursorScreenPos();
			unsigned int texId = sceneFbo->GetTextureID();
			if (texId != 0) {
				gui.Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)), static_cast<float>(w), static_cast<float>(h), 0.0f, 1.0f, 1.0f, 0.0f);
			}

			int gizmoHitAxis = -1;
			if (editor.GetMode() == EditorMode::Editor &&
				editor.GetSelectedEntityId() != NullEntity && editor.IsSceneViewFocused()) {
				if (ImGui::IsKeyPressed(ImGuiKey_W)) {
					editor.SetGizmoOperation(GizmoOperation::Translate);
				}
				if (ImGui::IsKeyPressed(ImGuiKey_E)) {
					editor.SetGizmoOperation(GizmoOperation::Rotate);
				}
				if (ImGui::IsKeyPressed(ImGuiKey_R)) {
					editor.SetGizmoOperation(GizmoOperation::Scale);
				}
			}

			if (editor.GetMode() == EditorMode::Editor &&
				editor.GetSelectedEntityId() != NullEntity && ImGui::IsItemHovered()) {
				auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
				if (scene && editor.GetGizmoOperation() == GizmoOperation::Translate) {
					Math::Mat4 viewMat, projMat;
					if (editor.GetEditorViewProjection(viewMat, projMat, aspect)) {
						World& world = scene->GetWorld();
						EntityId sel = editor.GetSelectedEntityId();
						auto* transform = world.GetComponent<TransformComponent>(sel);
						auto* localToWorld = world.GetComponent<LocalToWorldComponent>(sel);
						if (transform && world.IsAlive(sel)) {
							Math::Vec3 drawPos = transform->Position;
							if (localToWorld) {
								drawPos = Math::Vec3(localToWorld->Matrix.data[12], localToWorld->Matrix.data[13], localToWorld->Matrix.data[14]);
							}
							ImDrawList* drawList = ImGui::GetWindowDrawList();
							if (drawList) {
								ImVec2 mousePos = ImGui::GetMousePos();
								gizmoHitAxis = DrawTranslateGizmo(drawList, transform, drawPos, viewMat, projMat,
									imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h),
									mousePos.x, mousePos.y, ImGui::IsMouseDown(0));
							}
						}
					}
				}
			}

			if (editor.GetMode() == EditorMode::Editor &&
				ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) && gizmoHitAxis < 0) {
				auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
				if (scene) {
					Math::Mat4 viewMat, projMat;
					if (editor.GetEditorViewProjection(viewMat, projMat, aspect)) {
						ImVec2 mousePos = ImGui::GetMousePos();
						EntityId hit = PickEntityInViewport(*scene,
							mousePos.x, mousePos.y,
							imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h),
							viewMat, projMat);
						if (hit != NullEntity) {
							editor.SetSelectedEntity(hit);
						}
					}
				}
			}

			if (editor.GetMode() == EditorMode::Editor &&
				editor.GetSelectedEntityId() != NullEntity) {
				auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
				if (scene) {
					Math::Mat4 viewMat, projMat;
					if (editor.GetEditorViewProjection(viewMat, projMat, aspect)) {
						ImDrawList* drawList = ImGui::GetWindowDrawList();
						if (drawList) {
							DrawSelectionAABB(drawList, scene->GetWorld(), editor.GetSelectedEntityId(),
								viewMat, projMat,
								imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h));
						}
					}
				}
			}
		}
		ImGui::End();
	}

}
