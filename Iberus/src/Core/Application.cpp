#include "Enginepch.h"
#include "Application.h"
#include "Layer.h"

#include "RenderCmd.h"
#include "Renderer.h"
#include "Engine.h"
#include "ImguiContext.h"

#include <chrono>

namespace Iberus {

#define BIND_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		s_Instance = this;
		static WindowProps winProps{};
		window = std::unique_ptr<Window>(Window::Create(winProps));
		window->SetEventCallback(BIND_FN(Application::OnEvent));
		engine = Engine::Instance();
		engine->SetCurrentWindow(window.get());
	}

	Application::~Application() {
		s_Instance = nullptr;
	}

	void Application::Boot() {
		engine->Boot();

		guiContext = std::make_unique<ImguiContext>();
		guiContext->Init(window->GetNativeWindow());
	}

	void Application::OnEvent(Event& event) {
		engine->GetInputManager().OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_FN(Application::OnWindowResize));

		//IB_CORE_TRACE("{}", event.ToString());
	}

	bool Application::OnWindowResize(WindowResizeEvent& event) {
		engine->OnWindowResize(event.GetWidth(), event.GetHeight());
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event) {
		running = false;
		return true;
	}

	void Application::Update() {
	}

	void Application::PushLayer(Layer* layer) {
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		layerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	IGUIContext* Application::GetGUIContext() {
		return guiContext.get();
	}

	void Application::Run() {
		using Clock = std::chrono::high_resolution_clock;
		auto lastFrameTime = Clock::now();

		while (running) {
			auto now = Clock::now();
			double deltaTime = std::chrono::duration<double>(now - lastFrameTime).count();
			lastFrameTime = now;

			guiContext->BeginFrame();
			layerStack.ForEachLayer([deltaTime](Layer* layer) {
				layer->OnUpdate(deltaTime);
			});
			guiContext->EndFrame();

			Update();
			window->Update();
		}
	}
}