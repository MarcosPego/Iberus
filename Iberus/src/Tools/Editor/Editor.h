#pragma once

#include "IGUIContext.h"
#include "SceneTreePanel.h"
#include "SceneViewPanel.h"
#include "InspectorPanel.h"
#include "EntityId.h"

namespace Iberus {

	class Scene;

	enum class EditorMode { Editor, Game };

	class IBERUS_API Editor {
	public:
		Editor();
		~Editor();

		void OnUpdate(double deltaTime, IGUIContext* gui);

		EditorMode GetMode() const { return editorMode; }
		void SetMode(EditorMode mode) { editorMode = mode; }

		void SetSelectedEntity(EntityId entityId) { selectedEntityId = entityId; }
		EntityId GetSelectedEntityId() const { return selectedEntityId; }

	private:
		std::unique_ptr<SceneTreePanel> sceneTreePanel;
		std::unique_ptr<SceneViewPanel> sceneViewPanel;
		std::unique_ptr<InspectorPanel> inspectorPanel;
		EditorMode editorMode{ EditorMode::Editor };
		bool wasF11Down{ false };
		EntityId selectedEntityId{ NullEntity };
	};

}
