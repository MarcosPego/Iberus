#include "Enginepch.h"
#include "ImguiContext.h"
#include "FileSystem.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"

#include <GLFW/glfw3.h>
#include <filesystem>

namespace Iberus {

	ImguiContext::~ImguiContext() {
		Shutdown();
	}

	bool ImguiContext::Init(void* nativeWindow) {
		if (initialized) {
			return true;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Load default font and merge Font Awesome icons
		io.Fonts->AddFontDefault();
		std::string fontPath = FileSystem::GetAppDirectory() + "/fonts/fa-solid-900.ttf";
		if (std::filesystem::exists(fontPath)) {
			ImFontConfig config;
			config.MergeMode = true;
			config.GlyphMinAdvanceX = 13.0f;
			static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 13.0f, &config, iconRanges);
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
		if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
			return false;
		}
		if (!ImGui_ImplOpenGL3_Init("#version 460")) {
			ImGui_ImplGlfw_Shutdown();
			return false;
		}

		this->nativeWindow = nativeWindow;
		initialized = true;
		return true;
	}

	void ImguiContext::Shutdown() {
		if (!initialized) {
			return;
		}

		nativeWindow = nullptr;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		initialized = false;
	}

	void ImguiContext::BeginFrame() {
		if (!initialized) {
			return;
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	bool ImguiContext::IsMinimized() const {
		if (!initialized || !nativeWindow) {
			return false;
		}
		return glfwGetWindowAttrib(static_cast<GLFWwindow*>(nativeWindow), GLFW_ICONIFIED) == GLFW_TRUE;
	}

	void ImguiContext::EndFrame(bool render, bool clearBackdrop) {
		if (!initialized) {
			return;
		}
		ImGui::EndFrame();
		if (render) {
			ImGui::Render();
			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData) {
				int w = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
				int h = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
				if (w > 0 && h > 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					glViewport(0, 0, w, h);
					if (clearBackdrop) {
						glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					}
				}
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	bool ImguiContext::BeginWindow(const char* title, bool* p_open) {
		return ImGui::Begin(title, p_open);
	}

	void ImguiContext::EndWindow() {
		ImGui::End();
	}

	void ImguiContext::Text(const char* text) {
		ImGui::Text("%s", text);
	}

	bool ImguiContext::Button(const char* label) {
		return ImGui::Button(label);
	}

	void ImguiContext::BeginDockSpace() {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
	}

	void ImguiContext::EndDockSpace() {
	}

	void ImguiContext::Image(void* textureId, float w, float h, float uv0x, float uv0y, float uv1x, float uv1y) {
		ImGui::Image(reinterpret_cast<ImTextureID>(textureId), ImVec2(w, h), ImVec2(uv0x, uv0y), ImVec2(uv1x, uv1y));
	}

}
