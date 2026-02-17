#pragma once

#include "Core.h"
#include <memory>
#include "Window.h"
#include "Renderer.h"
#include "EventImport.h"
#include "LayerStack.h"
#include "IGUIContext.h"
#include "Project.h"

namespace Iberus {
	class Engine;
	class ImguiContext;

	class IBERUS_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void Boot();
		virtual void Update();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Run();

		static Application* Get() { return s_Instance; }
		IGUIContext* GetGUIContext();

		bool IsEditorMode() const { return editorMode; }
		void SetEditorMode(bool value) { editorMode = value; }

		bool ShouldShowProjectScreen() const { return showProjectScreen; }
		void DismissProjectScreen() { showProjectScreen = false; }
		void RequestProjectScreen() { showProjectScreen = true; }
		Project* GetProject() { return project.get(); }
		const Project* GetProject() const { return project.get(); }
		void SetProject(std::unique_ptr<Project> p) { project = std::move(p); }

		virtual void OnProjectChosen(Project* openedProject) { (void)openedProject; }

		const std::string& GetCurrentScenePath() const { return currentScenePath; }
		void SetCurrentScenePath(const std::string& path) { currentScenePath = path; }
		bool IsSceneDirty() const { return sceneDirty; }
		void SetSceneDirty(bool dirty) { sceneDirty = dirty; }

		virtual void OnCloseProject() {}

	private:
		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

		static Application* s_Instance;
		Engine* engine{ nullptr };
		std::unique_ptr<Window> window;
		std::unique_ptr<ImguiContext> guiContext;
		LayerStack layerStack;

		bool running{ true };
		bool editorMode{ true };
		bool showProjectScreen{ true };
		std::unique_ptr<Project> project;
		std::string currentScenePath;
		bool sceneDirty{ false };
	};

	Application* CreateApplication();
}


