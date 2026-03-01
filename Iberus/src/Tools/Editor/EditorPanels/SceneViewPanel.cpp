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
#include "IconsFontAwesome6.h"

#include "imgui.h"

#include <cmath>

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

		int DrawScaleGizmo(ImDrawList* drawList, TransformComponent* transform, const Math::Vec3& worldPos,
			const Math::Mat4& view, const Math::Mat4& proj,
			float vpX, float vpY, float vpW, float vpH,
			float mouseX, float mouseY, bool mouseDown) {
			static int draggingAxis = -1;
			static float prevMouseX = 0, prevMouseY = 0;
			static float prevScale = 1.0f;

			Math::Vec3 pos = worldPos;
			Math::Vec3 axes[3] = { {1,0,0}, {0,1,0}, {0,0,1} };
			ImU32 colors[3] = { IM_COL32(200,60,60,255), IM_COL32(60,200,60,255), IM_COL32(60,60,200,255) };

			float origSx, origSy;
			if (!WorldToScreen(pos, view, proj, vpX, vpY, vpW, vpH, origSx, origSy)) {
				return -1;
			}

			float endSx[3], endSy[3];
			for (int i = 0; i < 3; ++i) {
				float s = (transform->Scale.x + transform->Scale.y + transform->Scale.z) / 3.0f;
				Math::Vec3 end = pos + axes[i] * GizmoAxisLength * std::max(s, 0.1f);
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
					float scaleFactor = 1.0f + deltaPixels * 0.01f;
					float newScale = prevScale * scaleFactor;
					if (newScale < 0.01f) {
						newScale = 0.01f;
					}
					float oldAvg = (transform->Scale.x + transform->Scale.y + transform->Scale.z) / 3.0f;
					float ratio = oldAvg > 1e-6f ? newScale / oldAvg : 1.0f;
					transform->Scale.x *= ratio;
					transform->Scale.y *= ratio;
					transform->Scale.z *= ratio;
					prevScale = (transform->Scale.x + transform->Scale.y + transform->Scale.z) / 3.0f;
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
					prevScale = (transform->Scale.x + transform->Scale.y + transform->Scale.z) / 3.0f;
				} else if (!mouseDown) {
					draggingAxis = -1;
				}
			}
			prevMouseX = mouseX;
			prevMouseY = mouseY;
			return hitAxis;
		}

		int DrawRotateGizmo(ImDrawList* drawList, TransformComponent* transform, const Math::Vec3& worldPos,
			const Math::Mat4& view, const Math::Mat4& proj,
			float vpX, float vpY, float vpW, float vpH,
			float mouseX, float mouseY, bool mouseDown) {
			static int draggingAxis = -1;
			static float prevAngle = 0;
			static float prevMouseX = 0, prevMouseY = 0;

			Math::Vec3 pos = worldPos;
			ImU32 colors[3] = { IM_COL32(200,60,60,255), IM_COL32(60,200,60,255), IM_COL32(60,60,200,255) };

			float origSx, origSy;
			if (!WorldToScreen(pos, view, proj, vpX, vpY, vpW, vpH, origSx, origSy)) {
				return -1;
			}

			const int circleSegments = 32;
			const float circleRadius = GizmoAxisLength;
			float bestDist = GizmoAxisHitRadius * 2.0f;
			int hitAxis = -1;

			for (int axis = 0; axis < 3; ++axis) {
				float prevSx = 0, prevSy = 0;
				for (int i = 0; i <= circleSegments; ++i) {
					float angle = static_cast<float>(i) / circleSegments * 2.0f * 3.14159265f;
					Math::Vec3 p = pos;
					if (axis == 0) {
						p.x += circleRadius * cosf(angle);
						p.z += circleRadius * sinf(angle);
					} else if (axis == 1) {
						p.x += circleRadius * cosf(angle);
						p.y += circleRadius * sinf(angle);
					} else {
						p.y += circleRadius * cosf(angle);
						p.z += circleRadius * sinf(angle);
					}
					float sx, sy;
					if (WorldToScreen(p, view, proj, vpX, vpY, vpW, vpH, sx, sy)) {
						if (i > 0) {
							drawList->AddLine(ImVec2(prevSx, prevSy), ImVec2(sx, sy), colors[axis], 2.0f);
						}
						prevSx = sx;
						prevSy = sy;
					}
				}

				for (int i = 0; i < circleSegments; ++i) {
					float angle = static_cast<float>(i) / circleSegments * 2.0f * 3.14159265f;
					Math::Vec3 p = pos;
					if (axis == 0) {
						p.x += circleRadius * cosf(angle);
						p.z += circleRadius * sinf(angle);
					} else if (axis == 1) {
						p.x += circleRadius * cosf(angle);
						p.y += circleRadius * sinf(angle);
					} else {
						p.y += circleRadius * cosf(angle);
						p.z += circleRadius * sinf(angle);
					}
					float sx, sy;
					if (WorldToScreen(p, view, proj, vpX, vpY, vpW, vpH, sx, sy)) {
						float dx = mouseX - sx;
						float dy = mouseY - sy;
						float d = sqrtf(dx * dx + dy * dy);
						if (d < bestDist) {
							bestDist = d;
							hitAxis = axis;
						}
					}
				}
			}

			if (mouseDown && draggingAxis >= 0) {
				hitAxis = draggingAxis;
				float dx = mouseX - origSx;
				float dy = mouseY - origSy;
				float angle = atan2f(dy, dx);
				float prevAngleVal = atan2f(prevMouseY - origSy, prevMouseX - origSx);
				float deltaAngle = (angle - prevAngleVal) * 57.2957795f;
				if (hitAxis == 0) {
					transform->Rotation.x += deltaAngle;
				} else if (hitAxis == 1) {
					transform->Rotation.y += deltaAngle;
				} else {
					transform->Rotation.z += deltaAngle;
				}
			} else {
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (!ImGui::Begin("Scene View", p_open, ImGuiWindowFlags_NoNavInputs)) {
			editor.SetSceneViewFocused(false);
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}
		editor.SetSceneViewFocused(ImGui::IsWindowHovered());

		if (Application::Get()->IsGameFullscreen()) {
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}

		if (ImGui::IsWindowCollapsed()) {
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}

		ImVec2 size = ImGui::GetContentRegionAvail();
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);
		const int minViewportSize = 8;
		if (w < minViewportSize || h < minViewportSize || w > 16384 || h > 16384) {
			ImGui::End();
			ImGui::PopStyleVar();
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
				if (scene) {
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
								bool mouseDown = ImGui::IsMouseDown(0);
								if (editor.GetGizmoOperation() == GizmoOperation::Translate) {
									gizmoHitAxis = DrawTranslateGizmo(drawList, transform, drawPos, viewMat, projMat,
										imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h),
										mousePos.x, mousePos.y, mouseDown);
								} else if (editor.GetGizmoOperation() == GizmoOperation::Scale) {
									gizmoHitAxis = DrawScaleGizmo(drawList, transform, drawPos, viewMat, projMat,
										imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h),
										mousePos.x, mousePos.y, mouseDown);
								} else if (editor.GetGizmoOperation() == GizmoOperation::Rotate) {
									gizmoHitAxis = DrawRotateGizmo(drawList, transform, drawPos, viewMat, projMat,
										imageMin.x, imageMin.y, static_cast<float>(w), static_cast<float>(h),
										mousePos.x, mousePos.y, mouseDown);
								}
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

			if (editor.GetMode() == EditorMode::Editor) {
				ImGui::SetCursorScreenPos(ImVec2(imageMin.x + static_cast<float>(w) - 110.0f, imageMin.y + 8.0f));
				bool ortho = editor.GetEditorCamera().IsOrthographic;
				if (ImGui::Checkbox(ICON_FA_VIDEO " Ortho##SceneViewOrtho", &ortho)) {
					editor.GetEditorCamera().IsOrthographic = ortho;
				}
				if (ortho) {
					ImGui::SetCursorScreenPos(ImVec2(imageMin.x + static_cast<float>(w) - 110.0f, imageMin.y + 36.0f));
					float orthoSize = editor.GetEditorCamera().OrthoSize;
					if (ImGui::SliderFloat("##OrthoSize", &orthoSize, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
						editor.GetEditorCamera().OrthoSize = orthoSize;
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

}
