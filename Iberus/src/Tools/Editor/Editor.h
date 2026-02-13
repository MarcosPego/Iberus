#pragma once

#include "IGUIContext.h"
#include "SceneTreePanel.h"
#include "SceneViewPanel.h"
#include "InspectorPanel.h"

namespace Iberus {

	class Entity;

	enum class EditorMode { Editor, Game };

	class IBERUS_API Editor {
	public:
		Editor();
		~Editor();

		void OnUpdate(double deltaTime, IGUIContext* gui);

		EditorMode GetMode() const { return editorMode; }
		void SetMode(EditorMode mode) { editorMode = mode; }

		Entity* GetSelectedEntity() const { return selectedEntity; }
		void SetSelectedEntity(Entity* entity) { selectedEntity = entity; }

	private:
		std::unique_ptr<SceneTreePanel> sceneTreePanel;
		std::unique_ptr<SceneViewPanel> sceneViewPanel;
		std::unique_ptr<InspectorPanel> inspectorPanel;
		EditorMode editorMode{ EditorMode::Editor };
		bool wasF11Down{ false };
		Entity* selectedEntity{ nullptr };
	};

}
