#pragma once

#include "IGUIContext.h"
#include "SceneTreePanel.h"
#include "SceneViewPanel.h"
#include "InspectorPanel.h"
#include "CreatureCreatorPanel.h"
#include "FileSystemPanel.h"
#include "AssetInspectorPanel.h"
#include "EntityId.h"
#include "MathUtils.h"
#include "RenderCmd.h"

#include <memory>
#include <string>

namespace Iberus {

	class Scene;

	enum class EditorMode { Editor, Game };

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

		void SetViewportFocused(bool focused) { viewportFocused = focused; }
		bool IsViewportFocused() const { return viewportFocused; }

		EditorCameraState& GetEditorCamera() { return editorCamera; }
		const EditorCameraState& GetEditorCamera() const { return editorCamera; }

		/// Returns CameraRenderCmd for editor viewport when in Edit mode, or nullopt when in Game mode.
		std::unique_ptr<CameraRenderCmd> GetEditorCameraOverride() const;

	private:
		void UpdateEditorCamera(double deltaTime);

		std::unique_ptr<SceneTreePanel> sceneTreePanel;
		std::unique_ptr<SceneViewPanel> sceneViewPanel;
		std::unique_ptr<InspectorPanel> inspectorPanel;
		std::unique_ptr<CreatureCreatorPanel> creatureCreatorPanel;
		std::unique_ptr<FileSystemPanel> fileSystemPanel;
		std::unique_ptr<AssetInspectorPanel> assetInspectorPanel;
		EditorMode editorMode{ EditorMode::Editor };
		std::string selectedAssetPath;
		bool wasF11Down{ false };
		EntityId selectedEntityId{ NullEntity };
		EntityId copiedEntityId{ NullEntity };
		bool viewportFocused{ false };
		EditorCameraState editorCamera;

		// Editor camera controller state
		float editorCameraPitch{ 0 };
		float editorCameraYaw{ 180 };
		bool editorCameraDragging{ false };
		Math::Vec2 editorCameraLastMouse{ 0, 0 };
	};
}
