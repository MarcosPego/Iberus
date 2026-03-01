#include "Enginepch.h"
#include "Editor.h"
#include "Application.h"
#include "Engine.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
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
		float cosPitch = cosf(pitchRad);
		float sinPitch = sinf(pitchRad);
		float cosYaw = cosf(yawRad);
		float sinYaw = sinf(yawRad);

		Vec3 forward(sinf(yawRad) * cosf(pitchRad), -sinf(pitchRad), -cosf(yawRad) * cosf(pitchRad));
		forward = normalize(forward);
		Vec3 up(0, 1, 0);
		Vec3 center = pos + forward;
		Mat4 viewMatrix = MatrixFactory::CreateViewMat4(pos, center, up);
		Mat4 projectionMatrix = MatrixFactory::CreatePerspectiveMat4(
			editorCamera.Fovy, aspectRatio, editorCamera.NearZ, editorCamera.FarZ);
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
		outProj = MatrixFactory::CreatePerspectiveMat4(
			editorCamera.Fovy, aspectRatio, editorCamera.NearZ, editorCamera.FarZ);
		return true;
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
			if (ImGui::BeginMenu("Settings##EditorSettingsMenu")) {
				if (ImGui::MenuItem("Render Settings##EditorRenderSettings")) {
					renderSettingsPanel->RequestOpen();
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

		sceneTreePanel->OnDraw(*gui);
		sceneViewPanel->OnDraw(*gui);
		gameViewPanel->OnDraw(*gui);
		inspectorPanel->OnDraw(*gui);
		creatureCreatorPanel->OnDraw(*gui);
		fileSystemPanel->OnDraw(*gui);
		assetInspectorPanel->OnDraw(*gui);
		profilerPanel->OnDraw(*gui);
		renderSettingsPanel->OnDraw(*gui);
		gui->EndDockSpace();

		UpdateEditorCamera(deltaTime);
	}

}
