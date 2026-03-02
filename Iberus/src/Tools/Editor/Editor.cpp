#include "Enginepch.h"
#include "Editor.h"
#include "Application.h"
#include "Engine.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "EditorPicking.h"
#include "KeyCode.h"
#include "MouseCode.h"
#include "Matrix.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "IconsFontAwesome6.h"

using namespace Math;

namespace Iberus {

	Editor::Editor()
		: welcomePanel(std::make_unique<WelcomePanel>(*Application::Get()))
		, sceneTreePanel(std::make_unique<SceneTreePanel>(*this))
		, sceneViewPanel(std::make_unique<SceneViewPanel>(*this))
		, gameViewPanel(std::make_unique<GameViewPanel>(*this))
		, inspectorPanel(std::make_unique<InspectorPanel>(*this))
		, creatureCreatorPanel(std::make_unique<CreatureCreatorPanel>(*this))
		, fileSystemPanel(std::make_unique<FileSystemPanel>(*this))
		, assetInspectorPanel(std::make_unique<AssetInspectorPanel>(*this))
		, profilerPanel(std::make_unique<ProfilerPanel>(*this))
		, renderSettingsPanel(std::make_unique<RenderSettingsPanel>(*this)) {
	}

	Editor::~Editor() = default;

	void Editor::UpdateEditorCamera(double deltaTime) {
		if (!sceneViewFocused) {
			return;
		}

		auto& input = Engine::Instance()->GetInputManager();

		const float mouseSensitivity = 0.15f;
		const float moveSpeed = 8.0f;

		// Mouse rotation (right mouse)
		if (input.IsMouseButtonPressed(MouseCode::Right)) {
			Vec2 currentMouse = input.GetMousePosition();
			if (!editorCameraDragging) {
				editorCameraDragging = true;
				editorCameraLastMouse = currentMouse;
			} else {
				Vec2 delta = currentMouse - editorCameraLastMouse;
				editorCameraYaw += delta.x * mouseSensitivity;
				editorCameraPitch += delta.y * mouseSensitivity;
				const float maxPitch = 89.0f;
				if (editorCameraPitch > maxPitch) {
					editorCameraPitch = maxPitch;
				}
				if (editorCameraPitch < -maxPitch) {
					editorCameraPitch = -maxPitch;
				}
				editorCamera.Rotation = Vec3(editorCameraPitch, editorCameraYaw, 0.0f);
				editorCameraLastMouse = currentMouse;
			}
		} else {
			editorCameraDragging = false;
		}

		// Left-click pan (Hand tool)
		if (gizmoOperation == GizmoOperation::Hand && input.IsMouseButtonPressed(MouseCode::Left)) {
			Vec2 currentMouse = input.GetMousePosition();
			if (!editorCameraPanning) {
				editorCameraPanning = true;
				editorCameraLastMouse = currentMouse;
			} else {
				Vec2 delta = currentMouse - editorCameraLastMouse;
				float pitchRad = Deg2Rad(editorCameraPitch);
				float yawRad = Deg2Rad(editorCameraYaw);
				Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
				forward = normalize(forward);
				Vec3 right = normalize(cross(forward, Vec3(0, 1, 0)));
				Vec3 up = normalize(cross(right, forward));
				float panScale = editorCamera.IsOrthographic ? editorCamera.OrthoSize * 0.02f : 0.03f;
				editorCamera.Position = editorCamera.Position - right * (delta.x * panScale);
				editorCamera.Position = editorCamera.Position + up * (delta.y * panScale);
				editorCameraLastMouse = currentMouse;
			}
		} else {
			editorCameraPanning = false;
		}

		// Scroll wheel zoom
		{
			Vec2 scroll = input.GetScrollDelta();
			if (scroll.y != 0.0f) {
				float pitchRad = Deg2Rad(editorCameraPitch);
				float yawRad = Deg2Rad(editorCameraYaw);
				Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
				forward = normalize(forward);
				float zoomSpeed = 2.0f;
				if (editorCamera.IsOrthographic) {
					float delta = -scroll.y * editorCamera.OrthoSize * 0.1f;
					editorCamera.OrthoSize = std::max(0.5f, editorCamera.OrthoSize + delta);
				} else {
					float delta = scroll.y * zoomSpeed;
					editorCamera.Position = editorCamera.Position + forward * delta;
				}
			}
		}

		// WASD movement - forward/back along view direction, strafe left/right.
		{
			float pitchRad = Deg2Rad(editorCameraPitch);
			float yawRad = Deg2Rad(editorCameraYaw);
			Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
			forward = normalize(forward);
			Vec3 right = normalize(cross(forward, Vec3(0, 1, 0)));

			Vec3 movement(0, 0, 0);
			if (input.IsKeyPressed(KeyCode::W)) {
				movement += forward;
			}
			if (input.IsKeyPressed(KeyCode::S)) {
				movement -= forward;
			}
			if (input.IsKeyPressed(KeyCode::A)) {
				movement -= right;
			}
			if (input.IsKeyPressed(KeyCode::D)) {
				movement += right;
			}
			if (input.IsKeyPressed(KeyCode::E)) {
				movement.y += 1.0f;
			}
			if (input.IsKeyPressed(KeyCode::Q)) {
				movement.y -= 1.0f;
			}

			if (movement.length() > 0) {
				movement = normalize(movement);
				editorCamera.Position = editorCamera.Position + movement * moveSpeed * static_cast<float>(deltaTime);
			}
		}
	}

	std::unique_ptr<CameraRenderCmd> Editor::GetEditorCameraOverride(float aspectRatio) const {
		Vec3 pos = editorCamera.Position;
		Vec3 rot = editorCamera.Rotation;

		float pitchRad = Deg2Rad(rot.x);
		float yawRad = Deg2Rad(rot.y);
		Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
		forward = normalize(forward);
		Vec3 up(0, 1, 0);
		Vec3 center = pos + forward;
		Mat4 viewMatrix = MatrixFactory::CreateViewMat4(pos, center, up);
		Mat4 projectionMatrix;
		if (editorCamera.IsOrthographic) {
			float halfH = editorCamera.OrthoSize * 0.5f;
			float halfW = halfH * aspectRatio;
			projectionMatrix = MatrixFactory::CreateOrtoMat4(-halfW, halfW, -halfH, halfH, editorCamera.NearZ, editorCamera.FarZ);
		} else {
			projectionMatrix = MatrixFactory::CreatePerspectiveMat4(
				editorCamera.Fovy, aspectRatio, editorCamera.NearZ, editorCamera.FarZ);
		}
		Mat4 cameraToWorld = inverse(viewMatrix);

		return std::make_unique<CameraRenderCmd>(viewMatrix, projectionMatrix, pos, cameraToWorld);
	}

	bool Editor::GetEditorViewProjection(Math::Mat4& outView, Math::Mat4& outProj, float aspectRatio) const {
		Vec3 pos = editorCamera.Position;
		Vec3 rot = editorCamera.Rotation;
		float pitchRad = Deg2Rad(rot.x);
		float yawRad = Deg2Rad(rot.y);
		Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
		forward = normalize(forward);
		Vec3 up(0, 1, 0);
		Vec3 center = pos + forward;
		outView = MatrixFactory::CreateViewMat4(pos, center, up);
		if (editorCamera.IsOrthographic) {
			float halfH = editorCamera.OrthoSize * 0.5f;
			float halfW = halfH * aspectRatio;
			outProj = MatrixFactory::CreateOrtoMat4(-halfW, halfW, -halfH, halfH, editorCamera.NearZ, editorCamera.FarZ);
		} else {
			outProj = MatrixFactory::CreatePerspectiveMat4(
				editorCamera.Fovy, aspectRatio, editorCamera.NearZ, editorCamera.FarZ);
		}
		return true;
	}

	void Editor::FocusCameraOnEntity(EntityId entityId) {
		auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
		if (!scene || entityId == NullEntity || !scene->GetWorld().IsAlive(entityId)) {
			return;
		}
		Math::Vec3 aabbMin, aabbMax;
		GetEntityAABB(scene->GetWorld(), entityId, aabbMin, aabbMax);
		Math::Vec3 center = (aabbMin + aabbMax) * 0.5f;
		Math::Vec3 size = aabbMax - aabbMin;
		float maxDim = std::max({ size.x, size.y, size.z, 1.0f });
		float distance = maxDim * 2.5f;

		Math::Vec3 toCamera = editorCamera.Position - center;
		float len = toCamera.length();
		if (len < 1e-6f) {
			toCamera = Math::Vec3(0, 0, -1);
			len = 1.0f;
		} else {
			toCamera = toCamera / len;
		}
		float yawRad = atan2f(toCamera.x, -toCamera.z);
		float pitchRad = asinf(-toCamera.y);
		editorCameraPitch = Rad2Deg(pitchRad);
		editorCameraYaw = Rad2Deg(yawRad);
		editorCamera.Rotation = Math::Vec3(editorCameraPitch, editorCameraYaw, 0.0f);

		Math::Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
		forward = normalize(forward);
		editorCamera.Position = center - forward * distance;
	}

	void Editor::OnUpdate(double deltaTime, IGUIContext* gui) {
		auto& input = Engine::Instance()->GetInputManager();

		bool pDown = input.IsKeyPressed(KeyCode::P) && !input.IsKeyPressed(KeyCode::LeftShift) && !input.IsKeyPressed(KeyCode::RightShift);
		bool pPressed = pDown && !wasPDown;
		wasPDown = pDown;
		if (pPressed) {
			if (editorMode == EditorMode::Editor) {
				if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
					Buffer buf = SceneSerializer::Serialize(*scene, NullEntity);
					if (!buf.Invalid()) {
						playModeSnapshot.CopyDataFrom(buf);
					}
				}
				editorMode = EditorMode::Game;
				gamePaused = false;
				Engine::Instance()->OnSwitchedToGameMode();
			} else {
				if (!playModeSnapshot.Invalid()) {
					if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
						SceneSerializer::RestoreFromBuffer(*scene, playModeSnapshot);
					}
					SetSelectedEntity(NullEntity);
				}
				editorMode = EditorMode::Editor;
				gamePaused = false;
			}
		}

		bool f11Pressed = input.IsKeyPressed(KeyCode::F11) && !wasF11Down;
		wasF11Down = input.IsKeyPressed(KeyCode::F11);
		if (f11Pressed) {
			gameFullscreen = !gameFullscreen;
		}

		bool ctrlS = input.IsKeyPressed(KeyCode::S) && (input.IsKeyPressed(KeyCode::LeftControl) || input.IsKeyPressed(KeyCode::RightControl));
		if (ctrlS) {
			Application* app = Application::Get();
			if (!app->GetCurrentScenePath().empty()) {
				if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
					if (SceneSerializer::SaveToFile(*scene, app->GetCurrentScenePath())) {
						app->SetSceneDirty(false);
					}
				}
			}
		}

		if (!gui) {
			return;
		}

		if (Application::Get()->ShouldShowProjectScreen()) {
			welcomePanel->OnDraw(*gui);
			return;
		}

		if (gameFullscreen) {
			ImGui::Dummy(ImVec2(1, 1));
			UpdateEditorCamera(deltaTime);
			return;
		}

		bool hasScene = Engine::Instance()->GetSceneManager().GetActiveScene() != nullptr;

		// DockSpace must be submitted before any windows it hosts (menu bar creates a window)
		gui->BeginDockSpace();

		if (ImGui::BeginMainMenuBar()) {
			// Play / Stop / Pause / Step toolbar
			if (editorMode == EditorMode::Editor) {
				if (ImGui::Button("Play (P)##EditorToolbar") && hasScene) {
					if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
						Buffer buf = SceneSerializer::Serialize(*scene, NullEntity);
						if (!buf.Invalid()) {
							playModeSnapshot.CopyDataFrom(buf);
						}
					}
					editorMode = EditorMode::Game;
					gamePaused = false;
					Engine::Instance()->OnSwitchedToGameMode();
				}
			} else {
				if (ImGui::Button("Stop##EditorToolbar")) {
					if (!playModeSnapshot.Invalid()) {
						if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
							SceneSerializer::RestoreFromBuffer(*scene, playModeSnapshot);
						}
						SetSelectedEntity(NullEntity);
					}
					editorMode = EditorMode::Editor;
					gamePaused = false;
				}
				ImGui::SameLine();
				if (ImGui::Button(gamePaused ? "Resume##EditorToolbar" : "Pause##EditorToolbar") && hasScene) {
					gamePaused = !gamePaused;
				}
				ImGui::SameLine();
				if (ImGui::Button("Step##EditorToolbar") && gamePaused) {
					RequestStep();
				}
			}
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();
			if (ImGui::BeginMenu("View##EditorViewMenu")) {
				if (ImGui::MenuItem("Scene Tree##ViewSceneTree", nullptr, &sceneTreeOpen)) {}
				if (ImGui::MenuItem("Scene View##ViewSceneView", nullptr, &sceneViewOpen)) {}
				if (ImGui::MenuItem("Game##ViewGame", nullptr, &gameViewOpen)) {}
				if (ImGui::MenuItem("Inspector##ViewInspector", nullptr, &inspectorOpen)) {}
				if (ImGui::MenuItem("Creature Creator##ViewCreatureCreator", nullptr, &creatureCreatorOpen)) {}
				if (ImGui::MenuItem("Project##ViewProject", nullptr, &fileSystemOpen)) {}
				if (ImGui::MenuItem("Asset Inspector##ViewAssetInspector", nullptr, &assetInspectorOpen)) {}
				if (ImGui::MenuItem("Profiler##ViewProfiler", nullptr, &profilerOpen)) {}
				if (ImGui::MenuItem("Render Settings##ViewRenderSettings", nullptr, &renderSettingsOpen)) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings##EditorSettingsMenu")) {
				if (ImGui::MenuItem("Render Settings##EditorRenderSettings")) {
					renderSettingsPanel->RequestOpen();
					renderSettingsOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("File##EditorFileMenu")) {
				bool hasScenePath = !Application::Get()->GetCurrentScenePath().empty();
				bool hasScene = Engine::Instance()->GetSceneManager().GetActiveScene() != nullptr;
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save##EditorSave", "Ctrl+S", false, hasScene && hasScenePath)) {
					if (auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
						if (SceneSerializer::SaveToFile(*scene, Application::Get()->GetCurrentScenePath())) {
							Application::Get()->SetSceneDirty(false);
						}
					}
				}
				if (ImGui::MenuItem("Open Project##EditorOpenProject")) {
					Application::Get()->RequestProjectScreen();
				}
				if (ImGui::MenuItem("Close Project##EditorCloseProject")) {
					Application::Get()->OnCloseProject();
				}
				ImGui::EndMenu();
			}
			if (editorMode == EditorMode::Game) {
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), gamePaused ? " [Paused]" : " [Playing]");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), gameFullscreen ? " [F11: Exit Fullscreen]" : " [F11: Fullscreen]");
			}
			ImGui::EndMainMenuBar();
		}

		if (sceneTreeOpen) {
			sceneTreePanel->OnDraw(*gui, &sceneTreeOpen);
		}
		if (sceneViewOpen) {
			sceneViewPanel->OnDraw(*gui, &sceneViewOpen);
		}
		if (gameViewOpen) {
			gameViewPanel->OnDraw(*gui, &gameViewOpen);
		}
		if (inspectorOpen) {
			inspectorPanel->OnDraw(*gui, &inspectorOpen);
		}
		if (creatureCreatorOpen) {
			creatureCreatorPanel->OnDraw(*gui, &creatureCreatorOpen);
		}
		if (fileSystemOpen) {
			fileSystemPanel->OnDraw(*gui, &fileSystemOpen);
		}
		if (assetInspectorOpen) {
			assetInspectorPanel->OnDraw(*gui, &assetInspectorOpen);
		}
		if (profilerOpen) {
			profilerPanel->OnDraw(*gui, &profilerOpen);
		}
		if (renderSettingsOpen) {
			renderSettingsPanel->OnDraw(*gui, &renderSettingsOpen);
		}
		gui->EndDockSpace();

		UpdateEditorCamera(deltaTime);
	}

}
