#pragma once

#include "IGUIContext.h"
#include "SceneTreePanel.h"
#include "SceneViewPanel.h"
#include "GameViewPanel.h"
#include "InspectorPanel.h"
#include "CreatureCreatorPanel.h"
#include "FileSystemPanel.h"
#include "AssetInspectorPanel.h"
#include "ProfilerPanel.h"
#include "RenderSettingsPanel.h"
#include "WelcomePanel.h"
#include "EntityId.h"
#include "MathUtils.h"
#include "RenderCmd.h"
#include "Buffer.h"
#include "Matrix.h"

#include <memory>
#include <string>

namespace Iberus {

	class Scene;

	enum class EditorMode { Editor, Game };

	enum class GizmoOperation { Translate, Rotate, Scale };
	enum class GizmoMode { Local, World };

	/// Editor camera - separate from scene, used to render the viewport in Edit mode. Not in scene tree, never serialized.
	struct EditorCameraState {
		Math::Vec3 Position{ 0, 0, -5 };
		Math::Vec3 Rotation{ 0, 180, 0 };
		float Fovy{ 60.0f };
		float NearZ{ 0.1f };
		float FarZ{ 100000.0f };
	};

	class IBERUS_API Editor {
	public:
		Editor();
		~Editor();

		void OnUpdate(double deltaTime, IGUIContext* gui);

		EditorMode GetMode() const { return editorMode; }
		void SetMode(EditorMode mode) { editorMode = mode; }

		void SetSelectedEntity(EntityId entityId) { selectedEntityId = entityId; }
		EntityId GetSelectedEntityId() const { return selectedEntityId; }

		void SetCopiedEntity(EntityId entityId) { copiedEntityId = entityId; }
		EntityId GetCopiedEntityId() const { return copiedEntityId; }
		bool HasCopiedEntity() const { return copiedEntityId != NullEntity; }

		void SetSelectedAssetPath(const std::string& path) { selectedAssetPath = path; }
		const std::string& GetSelectedAssetPath() const { return selectedAssetPath; }

		void SetSceneViewFocused(bool focused) { sceneViewFocused = focused; }
		bool IsSceneViewFocused() const { return sceneViewFocused; }

		bool IsPaused() const { return gamePaused; }
		void SetPaused(bool paused) { gamePaused = paused; }

		bool IsGameFullscreen() const { return gameFullscreen; }
		void RequestStep() { stepRequested = true; }
		bool ConsumeStepRequest() { bool v = stepRequested; stepRequested = false; return v; }

		GizmoOperation GetGizmoOperation() const { return gizmoOperation; }
		void SetGizmoOperation(GizmoOperation op) { gizmoOperation = op; }
		GizmoMode GetGizmoMode() const { return gizmoMode; }
		void SetGizmoMode(GizmoMode mode) { gizmoMode = mode; }

		EditorCameraState& GetEditorCamera() { return editorCamera; }
		const EditorCameraState& GetEditorCamera() const { return editorCamera; }

		/// Returns editor camera for the given aspect ratio. Used by Scene viewport.
		std::unique_ptr<CameraRenderCmd> GetEditorCameraOverride(float aspectRatio) const;

		/// Get view and projection matrices for editor camera (for picking). Returns false if not available.
		bool GetEditorViewProjection(Math::Mat4& outView, Math::Mat4& outProj, float aspectRatio) const;

	private:
		void UpdateEditorCamera(double deltaTime);

		std::unique_ptr<WelcomePanel> welcomePanel;
		std::unique_ptr<SceneTreePanel> sceneTreePanel;
		std::unique_ptr<SceneViewPanel> sceneViewPanel;
		std::unique_ptr<GameViewPanel> gameViewPanel;
		std::unique_ptr<InspectorPanel> inspectorPanel;
		std::unique_ptr<CreatureCreatorPanel> creatureCreatorPanel;
		std::unique_ptr<FileSystemPanel> fileSystemPanel;
		std::unique_ptr<AssetInspectorPanel> assetInspectorPanel;
		std::unique_ptr<ProfilerPanel> profilerPanel;
		std::unique_ptr<RenderSettingsPanel> renderSettingsPanel;
		EditorMode editorMode{ EditorMode::Editor };
		std::string selectedAssetPath;
		bool wasF11Down{ false };
		bool wasPDown{ false };
		EntityId selectedEntityId{ NullEntity };
		EntityId copiedEntityId{ NullEntity };
		bool sceneViewFocused{ false };
		EditorCameraState editorCamera;
		Buffer playModeSnapshot;
		bool gamePaused{ false };
		bool gameFullscreen{ false };
		bool stepRequested{ false };
		GizmoOperation gizmoOperation{ GizmoOperation::Translate };
		GizmoMode gizmoMode{ GizmoMode::World };

		// Editor camera controller state
		float editorCameraPitch{ 0 };
		float editorCameraYaw{ 180 };
		bool editorCameraDragging{ false };
		Math::Vec2 editorCameraLastMouse{ 0, 0 };
	};
}
