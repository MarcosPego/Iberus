#include "Enginepch.h"
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "KeyCode.h"
#include "MouseCode.h"
#include "Matrix.h"

#include "imgui.h"

using namespace Math;

namespace Iberus {

	Editor::Editor()
		: sceneTreePanel(std::make_unique<SceneTreePanel>(*this))
		, sceneViewPanel(std::make_unique<SceneViewPanel>(*this))
		, inspectorPanel(std::make_unique<InspectorPanel>(*this))
		, fileSystemPanel(std::make_unique<FileSystemPanel>(*this))
		, assetInspectorPanel(std::make_unique<AssetInspectorPanel>(*this)) {
	}

	Editor::~Editor() = default;

	void Editor::UpdateEditorCamera(double deltaTime) {
		if (editorMode != EditorMode::Editor || !viewportFocused) {
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

		// WASD movement - forward/back along view direction, strafe left/right
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

	std::unique_ptr<CameraRenderCmd> Editor::GetEditorCameraOverride() const {
		if (editorMode != EditorMode::Editor) {
			return nullptr;
		}

		float aspect = Engine::Instance()->GetEffectiveRenderAspectRatio();
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
			editorCamera.Fovy, aspect, editorCamera.NearZ, editorCamera.FarZ);
		Mat4 cameraToWorld = inverse(viewMatrix);

		return std::make_unique<CameraRenderCmd>(viewMatrix, projectionMatrix, pos, cameraToWorld);
	}

	void Editor::OnUpdate(double deltaTime, IGUIContext* gui) {
		auto& input = Engine::Instance()->GetInputManager();
		bool f11Down = input.IsKeyPressed(KeyCode::F11);
		bool f11Pressed = f11Down && !wasF11Down;
		wasF11Down = f11Down;
		if (f11Pressed) {
			editorMode = (editorMode == EditorMode::Editor) ? EditorMode::Game : EditorMode::Editor;
			if (editorMode == EditorMode::Game) {
				Engine::Instance()->OnSwitchedToGameMode();
			}
		}

		if (editorMode != EditorMode::Editor || !gui) {
			return;
		}
		gui->BeginDockSpace();
		sceneTreePanel->OnDraw(*gui);
		sceneViewPanel->OnDraw(*gui);
		inspectorPanel->OnDraw(*gui);
		fileSystemPanel->OnDraw(*gui);
		assetInspectorPanel->OnDraw(*gui);
		gui->EndDockSpace();

		UpdateEditorCamera(deltaTime);
	}

}
