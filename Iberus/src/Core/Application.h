#pragma once

#include "Core.h"
#include "Window.h"
#include "Renderer.h"
#include "EventImport.h"
#include "LayerStack.h"
#include "IGUIContext.h"

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
	};

	Application* CreateApplication();
}


